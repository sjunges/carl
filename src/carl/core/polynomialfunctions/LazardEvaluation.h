#pragma once

#include "FieldExtensions.h"

#ifdef USE_COCOA

namespace carl {

template<typename Rational, typename Poly>
class LazardEvaluation {
private:
	FieldExtensions<Rational,Poly> mFieldExtensions;
	Poly mLiftingPoly;
	
public:
	LazardEvaluation(const Poly& p): mLiftingPoly(p) {}
	
	void substitute(Variable v, const RealAlgebraicNumber<Rational>& r, bool divideZeroFactors = true) {
		auto red = mFieldExtensions.extend(v, r);
		Poly newPoly;
		if (red.first) {
			CARL_LOG_DEBUG("carl.lazard", "Substituting " << v << " by " << red.second);
			newPoly = mLiftingPoly.substitute(v, red.second);
		} else {
			CARL_LOG_DEBUG("carl.lazard", "Obtained reductor " << red.second);
			newPoly = mLiftingPoly.remainder(red.second);
		}
		while (carl::isZero(newPoly) && divideZeroFactors) {
			if (red.first) {
				mLiftingPoly = mLiftingPoly.quotient(v - red.second);
				newPoly = mLiftingPoly.substitute(v, red.second);
			} else {
				mLiftingPoly = mLiftingPoly.quotient(red.second);
				newPoly = mLiftingPoly.remainder(red.second);
			}
			CARL_LOG_DEBUG("carl.lazard", "Reducing to " << mLiftingPoly);
		}
		mLiftingPoly = newPoly;
		CARL_LOG_DEBUG("carl.lazard", "Remaining poly: " << mLiftingPoly);
	}
	
	const auto& getLiftingPoly() const {
		return mLiftingPoly;
	}
};

}

#endif
