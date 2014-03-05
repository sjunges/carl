/** 
 * @file  MultivariateGCD.h
 * @ingroup multirp
 * @ingroup gcd
 * @author Sebastian Junges
 *
 */

#pragma once

#include "MonomialOrdering.h"
#include "MultivariatePolynomialPolicy.h"

namespace carl
{
	
template<typename C, typename O, typename P>
class MultivariatePolynomial;
template<typename C>
class UnivariatePolynomial;


	
/**
 * @param a A multivariately represented polynomial.
 * @param b A multivariately represented polynomial.
 * @return The greatest common divisor of a and b.
 * @ingroup gcd
 * @ingroup multirp
 * @see MultivariatePolynomial
 * @see MultivariateGCD
 */
template<typename C, typename O, typename P>
MultivariatePolynomial<C,O,P> gcd(const MultivariatePolynomial<C,O,P>& a, const MultivariatePolynomial<C,O,P>& b);
	

/**
 * The result of a gcd calculation, as with the extended gcd.
 * @ingroup gcd
 */	
template<typename Coeff, typename Ordering, typename Policies>
struct GCDResult
{
	typedef MultivariatePolynomial<Coeff,Ordering,Policies> Polynomial;

	Polynomial AdivG;
	Polynomial BdivG;
	Polynomial gcd;
};
	
/**
 * A general object for gcd calculation of multivariate gcds.
 * @ingroup gcd
 * @ingroup multirp
 */
template<typename GCDCalculation, typename Coeff, typename Ordering= GrLexOrdering, typename Policies = StdMultivariatePolynomialPolicies<>>
class MultivariateGCD : GCDCalculation
{
	typedef GCDResult<Coeff,Ordering,Policies> Result;
	typedef MultivariatePolynomial<Coeff,Ordering,Policies> Polynomial;
	typedef UnivariatePolynomial<MultivariatePolynomial<Coeff,Ordering,Policies>> UnivReprPol;
	typedef UnivariatePolynomial<Coeff> UnivPol;
	
	const Polynomial& mp1;
	const Polynomial& mp2;
	
	public:
	MultivariateGCD(const MultivariatePolynomial<Coeff,Ordering,Policies>& p1,const MultivariatePolynomial<Coeff,Ordering,Policies>& p2)
	: GCDCalculation(), mp1(p1), mp2(p2)
	{
		
	}
	
	/**
	 * 
     * @return 
     */
	Polynomial calculate();
	
	private:
	/**
	 * Given the two polynomials, find a suitable main variable for gcd.
     * @param p1
     * @param p2
     * @return NoVariable if intersection is empty, otherwise some variable v which is in p1 and p2.
     */
	Variable getMainVar(const Polynomial p1, const Polynomial p2) const
	{
		// TODO find good heuristic.
		std::set<Variable> common;
		std::set<Variable> v1 = p1.gatherVariables();
		std::set<Variable> v2 = p2.gatherVariables();
		
		std::set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),
                  std::inserter(common,common.begin()));
		if(common.empty())
		{
			return Variable::NO_VARIABLE;
		}
		else
		{
			return *common.begin();
		}
		
	}	
		
};



}
#include "PrimitiveEuclideanAlgorithm.h"
#include "MultivariateGCD.tpp"
#include "PrimitiveEuclideanAlgorithm.tpp"	

