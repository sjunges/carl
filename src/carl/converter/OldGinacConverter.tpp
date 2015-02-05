#include "../numbers/numbers.h"

namespace carl
{
    template<typename Poly>
    GiNaC::ex OldGinacConverter<Poly>::convertToGinac(const Poly& poly, const std::map<carl::Variable, GiNaC::ex>& vars)
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        GiNaC::ex result = 0;
        for(auto term = poly.begin(); term != poly.end(); ++term)
        {
            GiNaC::ex factor = GiNaC::ex( GiNaC::numeric( carl::rationalize<cln::cl_RA>(PreventConversion<typename Poly::CoeffType>(term->coeff())) ) );
            if((*term).monomial())
            {
                for (auto it: *(term->monomial())) {
                    auto carlToGinacVar = vars.find(it.first);
                    assert(carlToGinacVar != vars.end());
                    factor *= GiNaC::pow(carlToGinacVar->second, it.second);
                }
            }
            result += factor;
        }
        return result;
    }

    template<typename Poly>
    Poly OldGinacConverter<Poly>::convertToCarl(const GiNaC::ex& _toConvert, const std::map<GiNaC::ex, carl::Variable, GiNaC::ex_is_less>& vars)
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        Poly result;
        GiNaC::ex ginacPoly = _toConvert.expand();
        if(GiNaC::is_exactly_a<GiNaC::add>(ginacPoly))
        {
            result = Poly((typename Poly::CoeffType)0);
            for(auto summand = ginacPoly.begin(); summand != ginacPoly.end(); ++summand)
            {
                const GiNaC::ex summandEx = *summand;
                if(GiNaC::is_exactly_a<GiNaC::mul>(summandEx))
                {
                    Poly carlSummand = Poly((typename Poly::CoeffType)1);
                    for(auto factor = summandEx.begin(); factor != summandEx.end(); ++factor)
                    {
                        const GiNaC::ex factorEx = *factor;
                        if(GiNaC::is_exactly_a<GiNaC::symbol>(factorEx))
                        {
                            auto iter = vars.find(factorEx);
                            assert(iter != vars.end());
                            carlSummand *= iter->second;
                        }
                        else if(GiNaC::is_exactly_a<GiNaC::numeric>(factorEx))
                        {
                            carlSummand *= carl::rationalize<typename Poly::CoeffType>(PreventConversion<cln::cl_RA>(cln::rationalize(cln::realpart(GiNaC::ex_to<GiNaC::numeric>(factorEx).to_cl_N()))));
                        }
                        else if(GiNaC::is_exactly_a<GiNaC::power>(factorEx))
                        {
                            assert(factorEx.nops() == 2);
                            GiNaC::ex exponent = *(++(factorEx.begin()));
                            assert(!exponent.info(GiNaC::info_flags::negative));
                            unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                            GiNaC::ex subterm = *factorEx.begin();
                            assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
                            auto iter = vars.find(subterm);
                            assert(iter != vars.end());
                            carlSummand *= typename Poly::TermType((typename Poly::CoeffType)1, iter->second, exp);
                        }
                        else assert(false);
                    }
                    result += carlSummand;
                }
                else if(GiNaC::is_exactly_a<GiNaC::symbol>(summandEx))
                {
                    auto iter = vars.find(summandEx);
                    assert(iter != vars.end());
                    result += iter->second;
                }
                else if(GiNaC::is_exactly_a<GiNaC::numeric>(summandEx))
                {
                    result += carl::rationalize<typename Poly::CoeffType>(PreventConversion<cln::cl_RA>(cln::rationalize(cln::realpart(GiNaC::ex_to<GiNaC::numeric>(summandEx).to_cl_N()))));
                }
                else if(GiNaC::is_exactly_a<GiNaC::power>(summandEx))
                {
                    assert(summandEx.nops() == 2);
                    GiNaC::ex exponent = *(++(summandEx.begin()));
                    assert(!exponent.info( GiNaC::info_flags::negative));
                    unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                    GiNaC::ex subterm = *summandEx.begin();
                    assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
                    auto iter = vars.find(subterm);
                    assert(iter != vars.end());
                    result += typename Poly::TermType((typename Poly::CoeffType)1, iter->second, exp);
                }
                else assert(false);
            }
        }
        else if(GiNaC::is_exactly_a<GiNaC::mul>(ginacPoly))
        {
            result = Poly((typename Poly::CoeffType)1);
            for(auto factor = ginacPoly.begin(); factor != ginacPoly.end(); ++factor)
            {
                const GiNaC::ex factorEx = *factor;
                if(GiNaC::is_exactly_a<GiNaC::symbol>(factorEx))
                {
                    auto iter = vars.find(factorEx);
                    assert(iter != vars.end());
                    result *= iter->second;
                }
                else if(GiNaC::is_exactly_a<GiNaC::numeric>(factorEx))
                {
                    result *= carl::rationalize<typename Poly::CoeffType>(PreventConversion<cln::cl_RA>(cln::rationalize(cln::realpart(GiNaC::ex_to<GiNaC::numeric>(factorEx).to_cl_N()))));
                }
                else if(GiNaC::is_exactly_a<GiNaC::power>(factorEx))
                {
                    assert(factorEx.nops() == 2);
                    GiNaC::ex exponent = *(++(factorEx.begin()));
                    assert(!exponent.info(GiNaC::info_flags::negative));
                    unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                    GiNaC::ex subterm = *factorEx.begin();
                    assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
                    auto iter = vars.find(subterm);
                    assert(iter != vars.end());
                    result *= typename Poly::TermType((typename Poly::CoeffType)1, iter->second, exp);
                }
                else assert( false );
            }
        }
        else if(GiNaC::is_exactly_a<GiNaC::symbol>(ginacPoly))
        {
            auto iter = vars.find(ginacPoly);
            assert(iter != vars.end());
            result = Poly(iter->second);
        }
        else if(GiNaC::is_exactly_a<GiNaC::numeric>(ginacPoly))
        {
            result = Poly(carl::rationalize<typename Poly::CoeffType>(PreventConversion<cln::cl_RA>(cln::rationalize(cln::realpart( GiNaC::ex_to<GiNaC::numeric>(ginacPoly).to_cl_N())))));
        }
        else if(GiNaC::is_exactly_a<GiNaC::power>(ginacPoly))
        {
            assert(ginacPoly.nops() == 2);
            GiNaC::ex exponent = *(++ginacPoly.begin());
            assert(!exponent.info(GiNaC::info_flags::negative));
            unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
            GiNaC::ex subterm = *ginacPoly.begin();
            assert(GiNaC::is_exactly_a<GiNaC::symbol>(subterm));
            auto iter = vars.find(subterm);
            assert(iter != vars.end());
            result = Poly(typename Poly::TermType((typename Poly::CoeffType)1, iter->second, exp));
        }
        else assert( false );
        return result;
    }

    template<typename Poly>
    Poly OldGinacConverter<Poly>::ginacGcd(const Poly& polyA, const Poly& polyB)
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        Poly result;
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(polyA, carlToGinacVarMap, ginacToCarlVarMap);
        gatherVariables(polyB, carlToGinacVarMap, ginacToCarlVarMap);
        GiNaC::ex ginacResult = GiNaC::gcd(convertToGinac(polyA, carlToGinacVarMap), convertToGinac(polyB, carlToGinacVarMap));
        result = convertToCarl(ginacResult, ginacToCarlVarMap);
        if( !result.isZero() && result.lcoeff() < carl::constant_zero<typename Poly::CoeffType>().get() )
            return -result;
        return result;
    }
    
    template<typename Poly>
    bool OldGinacConverter<Poly>::checkConversion(const Poly& polyA)
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(polyA, carlToGinacVarMap, ginacToCarlVarMap);
        Poly result = convertToCarl(convertToGinac(polyA, carlToGinacVarMap), ginacToCarlVarMap);
        return polyA == result;  
    }

    template<typename Poly>
    bool OldGinacConverter<Poly>::ginacDivide(const Poly& polyA, const Poly& polyB, Poly& result)
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(polyA, carlToGinacVarMap, ginacToCarlVarMap);
        gatherVariables(polyB, carlToGinacVarMap, ginacToCarlVarMap);
        GiNaC::ex ginacResult;
        bool divided = GiNaC::divide(convertToGinac(polyA, carlToGinacVarMap), convertToGinac(polyB, carlToGinacVarMap), ginacResult);
        if(divided)
            result = convertToCarl(ginacResult, ginacToCarlVarMap);
        return divided;
    }

    template<typename Poly>
    std::unordered_map<const Poly, unsigned, std::hash<Poly>> OldGinacConverter<Poly>::ginacFactorization(const Poly& poly)
    {
        std::lock_guard<std::recursive_mutex> lock( mMutex );
        std::unordered_map<const Poly, unsigned, std::hash<Poly>> result;
        std::map<Variable, GiNaC::ex> carlToGinacVarMap;
        std::map<GiNaC::ex, Variable, GiNaC::ex_is_less> ginacToCarlVarMap;
        gatherVariables(poly, carlToGinacVarMap, ginacToCarlVarMap);
        GiNaC::ex ginacResult = GiNaC::factor(convertToGinac(poly, carlToGinacVarMap));
        if(GiNaC::is_exactly_a<GiNaC::mul>(ginacResult))
        {
            for(auto factor = ginacResult.begin(); factor != ginacResult.end(); ++factor)
            {
                const GiNaC::ex& factorEx = *factor;
                if(GiNaC::is_exactly_a<GiNaC::power>(factorEx))
                {
                    assert(factorEx.nops() == 2);
                    GiNaC::ex exponent = *(++factorEx.begin());
                    assert(!exponent.info(GiNaC::info_flags::negative));
                    unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
                    GiNaC::ex subterm = *factorEx.begin();
                    Poly carlFactor = convertToCarl(subterm, ginacToCarlVarMap);
                    assert(result.find(carlFactor) == result.end());
                    result.insert(std::pair<Poly, unsigned>(carlFactor, exp));
                }
                else
                {
                    Poly carlFactor = convertToCarl(factorEx, ginacToCarlVarMap);
                    assert(result.find(carlFactor) == result.end());
                    result.insert(std::pair<Poly, unsigned>(carlFactor, 1));
                }
            }
        }
        else if(GiNaC::is_exactly_a<GiNaC::power>(ginacResult))
        {
            assert(ginacResult.nops() == 2);
            GiNaC::ex exponent = *(++ginacResult.begin());
            assert(!exponent.info(GiNaC::info_flags::negative));
            unsigned exp = static_cast<unsigned>(exponent.integer_content().to_int());
            GiNaC::ex subterm = *ginacResult.begin();
            result.insert(std::pair<Poly, unsigned>(convertToCarl(subterm, ginacToCarlVarMap), exp));
        }
        else
        {
            result.insert(std::pair<Poly, unsigned>(convertToCarl(ginacResult, ginacToCarlVarMap), 1));
        }
        return result;
    }
} // end namespace carl
