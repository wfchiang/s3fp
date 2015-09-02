#pragma once 

extern "C" {
#include <quadmath.h>
}
#include <assert.h>
#include <inttypes.h>
#include "boost/multiprecision/cpp_int.hpp"
#include <random>
#include <string>
using namespace std;
using namespace boost::multiprecision;


#ifndef S3FP_FP_UTILS 
#define S3FP_FP_UTILS 

// ---- utilities of decomposition of floating-point numbers ----
template<typename INTT> 
INTT MaxUint() {
  assert(sizeof(INTT) == 4 || 
	 sizeof(INTT) == 8 || 
	 sizeof(INTT) == 16); 

  INTT ret_int; 

  if (sizeof(INTT) == 4) 
    return 0xFFFFFFFF;
  else if (sizeof(INTT) == 8) 
    return 0xFFFFFFFFFFFFFFFF; 
  else if (sizeof(INTT) == 16) {
    uint64_t all_one[2];
    all_one[0] = all_one[1] = 0xFFFFFFFFFFFFFFFF; 
    uint128_t mask_ones; 
    memcpy(&mask_ones, all_one, 16); 
    return (INTT)mask_ones;     
  }
  else assert(false); 

  assert(false);
  return 0; 
}

template<typename FPT, typename INTT> 
void DecomposeIEEEFP (FPT fp, 
		      int &out_sign, 
		      INTT &out_expo, 
		      INTT &out_mant) {
  assert(sizeof(float) == 4 && 
	 sizeof(double) == 8 && 
	 sizeof(__float128) == 16); 
  assert(sizeof(FPT) == sizeof(INTT)); 
  assert(sizeof(FPT) == sizeof(float) || 
	 sizeof(FPT) == sizeof(double) ||
	 sizeof(FPT) == sizeof(__float128)); 

  INTT intfp; 
  int sign;
  INTT mant;
  INTT expo; 
  memcpy(&intfp, &fp, sizeof(FPT)); 

  INTT mask_mant; 
  INTT mask_expo; 

  switch (sizeof(INTT)) {
  case 4: 
    mask_mant = 0x7FFFFF; 
    mask_expo = 0xFF; 
    break;
  case 8:
    mask_mant = 0xFFFFFFFFFFFFF; 
    mask_expo = 0x7FF; 
    break;
  case 16:
    mask_mant = MaxUint<INTT>(); 
    mask_mant = mask_mant >> 16; // 0x0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF; 
    mask_expo = MaxUint<INTT>() >> 113; // 0x00000000000000000000000000007FFF;
    break; 
  default:
    assert(false);
    break;
  }

  mant = intfp & mask_mant;

  switch (sizeof(INTT)) {
  case 4:
    expo = (intfp >> 23) & mask_expo; 
    sign = (((intfp >> 31) == 1) ? 1 : 0); 
    break;
  case 8:
    expo = (intfp >> 52) & mask_expo; 
    sign = (((intfp >> 63) == 1) ? 1 : 0); 
    break;
  case 16:
    expo = (intfp >> 112) & mask_expo; 
    sign = (((intfp >> 127) == 1) ? 1 : 0); 
    break;
  default:
    assert(false); 
    break; 
  }

  assert(sign == 0 || sign == 1); 
  
  out_sign = sign; 
  out_mant = mant;
  out_expo = expo;   
}
/*
void DecomposeIEEEFP (unsigned int bit_width, 
		      __float128 in_fp, 
		      int &out_sign, 
		      uint128_t &out_expo, 
		      uint128_t &out_mant) {
  assert(bit_width == 32 ||
	 bit_width == 64 ||
	 bit_width == 128); 
  assert(sizeof(float) == 4 && sizeof(double) == 8 && sizeof(__float128) == 16);

  if (bit_width == 32) {
    float fp = in_fp;
    assert(fp == in_fp);

    uint32_t intfp; 
    int sign;
    uint32_t mant;
    uint32_t expo; 
    memcpy(&intfp, &fp, 4); 

    uint32_t mask_mant = 0x7FFFFF; 
    mant = intfp & mask_mant;
  
    uint32_t mask_expo = 0xFF; 
    expo = (intfp >> 23) & mask_expo; 
  
    if ((intfp >> 31) == 1) sign = 1;
    else sign = 0;

    out_sign = sign; 
    out_mant = mant;
    out_expo = expo;   
    assert(out_sign == sign && out_mant == mant && out_expo == expo); 
  }
  else if (bit_width == 64) {
    double fp = in_fp;
    assert(fp == in_fp);

    uint64_t intfp; 
    int sign;
    uint64_t mant;
    uint64_t expo; 
    memcpy(&intfp, &fp, 8); 

    uint64_t mask_mant = 0xFFFFFFFFFFFFF; 
    mant = intfp & mask_mant;
  
    uint64_t mask_expo = 0x7FF; 
    expo = (intfp >> 52) & mask_expo; 
    
    if ((intfp >> 63) == 1) sign = 1;
    else sign = 0;

    out_sign = sign; 
    out_mant = mant;
    out_expo = expo;   
    assert(out_sign == sign && out_mant == mant && out_expo == expo); 
  }
  else if (bit_width == 128) {
    __float128 fp = in_fp;
    assert(fp == in_fp);
    
    uint128_t intfp; 
    int sign;
    uint128_t mant;
    uint128_t expo; 
    memcpy(&intfp, &fp, 16); 

    uint128_t mask_ones = MaxUint128(); 
    uint128_t mask_mant = mask_ones >> 16; // 0x0000FFFFFFFFFFFFFFFFFFFFFFFFFFFF; 
    mant = intfp & mask_mant;
  
    uint128_t mask_expo = mask_ones >> 113; // 0x00000000000000000000000000007FFF;
    expo = (intfp >> 112) & mask_expo; 
  
    if ((intfp >> 127) == 1) sign = 1;
    else sign = 0;

    out_sign = sign; 
    out_mant = mant;
    out_expo = expo; 
    assert(out_sign == sign && out_mant == mant && out_expo == expo); 
  }
  else assert(false && "ERROR [DecomposeIEEEFP]: Impossible Case... \n");
}
*/


// ---- predicates for floating-point numbers ---- 
bool IsDenormalIEEEFP (int sign, 
		       uint128_t expo, 
		       uint128_t mant) {
  if (expo == 0 && mant != 0) return true;
  else return false;
}

bool IsZeroIEEEFP (int sign, 
		   uint128_t expo, 
		   uint128_t mant) {
  if (expo == 0 && mant == 0) return true;
  else return false;
}

bool IsInfIEEEFP (unsigned int bit_width, 
		  int sign, 
		  uint128_t expo, 
		  uint128_t mant) {
  switch (bit_width) {
  case 32:
    if (expo == 0xFF && mant == 0) return true;
    break;
  case 64:
    if (expo == 0x7FF && mant == 0) return true;
    break;
  case 128:
    if (expo == 0x7FF && mant == 0) return true;
    break;
  default:
    assert(false && "ERROR [IsInfIEEEFP]: Invalid bit_width \n"); 
    break;
  }
  return false;
}

bool IsNaNIEEEFP (unsigned int bit_width, 
		  int sign, 
		  uint128_t expo, 
		  uint128_t mant) {
  assert(sign == 0 || sign == 1);

  switch (bit_width) {
  case 32:
    if (expo == 0xFF && mant != 0) return true;
    break;
  case 64:
    if (expo == 0x7FF && mant != 0) return true;
    break;
  case 128:
    if (expo == 0x7FF && mant != 0) return true;
    break;
  default:
    assert(false && "ERROR [IsNaNIEEEFP]: Invalid bit_width \n"); 
    break;
  }
  return false;
}


// ---- utilities for creating new floating-point numbers ---- 
template<typename FPT, typename INTT> 
FPT MakeIEEEFP (int sign, 
		INTT expo,
		INTT mant) {
  assert(sizeof(float) == 4 && 
	 sizeof(double) == 8 && 
	 sizeof(__float128) == 16); 
  assert(sizeof(FPT) == sizeof(INTT)); 
  assert(sizeof(FPT) == sizeof(float) || 
	 sizeof(FPT) == sizeof(double) ||
	 sizeof(FPT) == sizeof(__float128)); 

  assert(sign == 0 || sign == 1); 
  
  INTT int_ret; 
  FPT fp_ret; 

  int_ret = sign;   
  int_ret = int_ret << (sizeof(INTT) * 8 - 1); 

  switch (sizeof(INTT)) {
  case 4:
    int_ret += (expo << 23);
    break;
  case 8:
    int_ret += (expo << 52); 
    break;
  case 16:
    int_ret += (expo << 112); 
    break;
  default:
    assert(false);
    break;
  }

  int_ret += mant; 
  memcpy(&fp_ret, &int_ret, sizeof(FPT)); 
  return fp_ret; 
}  
/*
__float128 MakeIEEEFP (unsigned int bit_width, 
		       int sign, 
		       uint128_t in_expo, 
		       uint128_t in_mant) {
  assert(sign == 0 || sign == 1);
  assert(bit_width == 32 ||
	 bit_width == 64 ||
	 bit_width == 128); 
  assert(sizeof(float) == 4 && sizeof(double) == 8 && sizeof(__float128) == 16);
  
  if (bit_width == 32) {
    uint32_t int_ret; 
    uint32_t expo = in_expo.convert_to<uint32_t>();
    uint32_t mant = in_mant.convert_to<uint32_t>();
    assert(expo == in_expo && mant == in_mant); 

    int_ret = sign; 
    int_ret = int_ret << 31; 
    int_ret += (expo << 23);
    int_ret += mant; 
    float fp_ret;
    memcpy(&fp_ret, &int_ret, 4); 
    return (__float128) fp_ret;
  }
  else if (bit_width == 64) {
    uint64_t int_ret; 
    uint64_t expo = in_expo.convert_to<uint64_t>();
    uint64_t mant = in_mant.convert_to<uint64_t>();
    assert(expo == in_expo && mant == in_mant); 
    
    int_ret = sign; 
    int_ret = int_ret << 63;
    int_ret += (expo << 52); 
    int_ret += mant; 
    double fp_ret;
    memcpy(&fp_ret, &int_ret, 8); 
    return (__float128) fp_ret;
  }
  else if (bit_width == 128) {
    uint128_t int_ret; 
    uint128_t expo = in_expo;
    uint128_t mant = in_mant; 
    assert(expo == in_expo && mant == in_mant); 
    
    int_ret = sign; 
    int_ret = int_ret << 127; 
    int_ret += (expo << 112); 
    int_ret += mant; 
    __float128 fp_ret;
    memcpy(&fp_ret, &int_ret, 16); 
    return fp_ret;
  }
  else assert(false && "ERROR [MakeIEEEFP]: Impossible Case... \n");
}
*/


// ---- calculate the numbers of floating-point numbers between two FPS ----
// Number of FPS between [low_fp, high_fp) 
template<typename FPT, typename INTT> 
INTT NFPSBetweenTwoIEEEFPS (FPT low_fp, 
			    FPT high_fp, 
			    bool include_denormal = false) { 
  assert(sizeof(float) == 4 && 
	 sizeof(double) == 8 && 
	 sizeof(__float128) == 16); 
  assert(sizeof(FPT) == sizeof(INTT)); 
  assert(sizeof(FPT) == sizeof(float) || 
	 sizeof(FPT) == sizeof(double) ||
	 sizeof(FPT) == sizeof(__float128)); 

  assert(low_fp <= high_fp);
  if (low_fp == high_fp) return 0;

  int bit_width = sizeof(INTT) * 8; 
  INTT n_fps = 0;
  
  int low_sign, high_sign;
  INTT low_mant, high_mant;
  INTT low_expo, high_expo;
  DecomposeIEEEFP<FPT, INTT>(low_fp, low_sign, low_expo, low_mant);
  DecomposeIEEEFP<FPT, INTT>(high_fp, high_sign, high_expo, high_mant);
  assert(low_sign == 1 || low_sign == 0);
  assert(high_sign == 1 || high_sign == 0);
  assert(IsInfIEEEFP(bit_width, low_sign, low_expo, low_mant) == false);
  assert(IsNaNIEEEFP(bit_width, low_sign, low_expo, high_mant) == false);
  assert(IsInfIEEEFP(bit_width, high_sign, high_expo, high_mant) == false);
  assert(IsNaNIEEEFP(bit_width, high_sign, high_expo, high_mant) == false);  

  if (high_fp == 0) {
    if (include_denormal) 
      return 1 + NFPSBetweenTwoIEEEFPS<FPT, INTT>(low_fp, 
						  MakeIEEEFP<FPT, INTT>(1, 0, 1), // the largest -denormal
						  true);
    else 
      return 1 + NFPSBetweenTwoIEEEFPS<FPT, INTT>(low_fp, 
						  MakeIEEEFP<FPT, INTT>(1, 1, 0), // the largest -normal
						  false);
  }
  else if (low_fp == 0) {
    if (include_denormal) 
      return 1 + NFPSBetweenTwoIEEEFPS<FPT, INTT>(MakeIEEEFP<FPT, INTT>(0, 0, 1), // the smallest +denormal
						  high_fp, 
						  true);
    else 
      return 1 + NFPSBetweenTwoIEEEFPS<FPT, INTT>(MakeIEEEFP<FPT, INTT>(0, 1, 0), // the smallest +normal
						  high_fp, 
						  false); 
  }
  else if (high_sign == 0 && low_sign == 1) {
    return NFPSBetweenTwoIEEEFPS<FPT, INTT>(0, high_fp, include_denormal) 
      + NFPSBetweenTwoIEEEFPS<FPT, INTT>(low_fp, 0, include_denormal);
  }
  else if (high_sign == 0 && low_sign == 0) {
    bool subn_high = IsDenormalIEEEFP(high_sign, high_expo, high_mant); 
    bool subn_low = IsDenormalIEEEFP(low_sign, low_expo, low_mant); 

    if ((subn_high == true) &&
	(subn_low == true)) {
      assert(high_mant >= low_mant);
      return high_mant - low_mant; 
    }
    else if ((subn_high == false) &&
	     (subn_low == true)) {
      n_fps = NFPSBetweenTwoIEEEFPS<FPT, INTT>(MakeIEEEFP<FPT, INTT>(0, 1, 0), // the smallest +normal
					       high_fp, 
					       include_denormal); 
      if (include_denormal) { 
	FPT new_high_fp; // the largest +denormal
	switch (bit_width) {
	case 32:
	  new_high_fp = MakeIEEEFP<FPT, INTT>(0, 0, 0x7FFFFF);
	  break;
	case 64:
	  new_high_fp = MakeIEEEFP<FPT, INTT>(0, 0, 0xFFFFFFFFFFFFF);
	  break;
	case 128:
	  new_high_fp = MakeIEEEFP<FPT, INTT>(0, 0, (MaxUint<INTT>() >> 16));
	  break;
	default:
	  assert(false && "ERROR: Impossible Case... \n");
	  break;
	}
	n_fps += (1 + NFPSBetweenTwoIEEEFPS<FPT, INTT>(low_fp, 
						       new_high_fp, 
						       include_denormal));
      }
     
      return n_fps;
    }
    else if ((subn_high == false) &&
	     (subn_low == false)) {
      if (high_expo == low_expo) {
	assert(high_mant >= low_mant);
	return (high_mant - low_mant);
      }
      else if (high_expo > low_expo) {
	FPT new_high_fp; 
	switch (bit_width) {
	case 32:
	  n_fps = ((high_expo - low_expo) - 1) << 23; 
	  new_high_fp = MakeIEEEFP<FPT, INTT>(0, low_expo, 0x7FFFFF); 
	  break;
	case 64:
	  n_fps = ((high_expo - low_expo) - 1) << 52; 
	  new_high_fp = MakeIEEEFP<FPT, INTT>(0, low_expo, 0xFFFFFFFFFFFFF);
	  break;
	case 128:
	  n_fps = ((high_expo - low_expo) - 1) << 112; 
	  new_high_fp = MakeIEEEFP<FPT, INTT>(0, low_expo, (MaxUint<INTT>() >> 16));
	  break;
	default:
	  assert(false && "ERROR: Impossible Case... \n");
	  break;
	}
	n_fps += NFPSBetweenTwoIEEEFPS<FPT, INTT>(MakeIEEEFP<FPT, INTT>(0, high_expo, 0), 
						  high_fp, 
						  include_denormal);
	n_fps += (1 + NFPSBetweenTwoIEEEFPS<FPT, INTT>(low_fp, 
						       new_high_fp, 
						       include_denormal));
      }
      else assert(false && "ERROR: Impossible Case... \n");
    }
    else assert(false && "ERROR: Impossible Case... \n");
  }
  else if (high_sign == 1 && low_sign == 1) {
    return NFPSBetweenTwoIEEEFPS<FPT, INTT>((-1)*high_fp, 
					    (-1)*low_fp, 
					    include_denormal);
  }
  else assert(false && "Error Impossible Case... \n");

  return n_fps; 
}
/*
uint128_t NFPSBetweenTwoIEEEFPS (unsigned int bit_width, 
				 __float128 low_fp, 
				 __float128 high_fp, 
				 bool include_denormal = false) { 
  assert(bit_width == 32 ||
	 bit_width == 64 ||
	 bit_width == 128); 
  assert(sizeof(float) == 4 && sizeof(double) == 8 && sizeof(__float128) == 16);

  assert(low_fp <= high_fp);
  if (low_fp == high_fp) return 0;

  uint128_t n_fps = 0;
  
  int low_sign, high_sign;
  uint128_t low_mant, high_mant;
  uint128_t low_expo, high_expo;
  DecomposeIEEEFP(bit_width, low_fp, low_sign, low_expo, low_mant);
  DecomposeIEEEFP(bit_width, high_fp, high_sign, high_expo, high_mant);
  assert(high_sign == 1 || high_sign == 0);
  assert(low_sign == 1 || low_sign == 0);
  assert(IsInfIEEEFP(bit_width, low_sign, low_expo, low_mant) == false);
  assert(IsNaNIEEEFP(bit_width, low_sign, low_expo, high_mant) == false);
  assert(IsInfIEEEFP(bit_width, high_sign, high_expo, high_mant) == false);
  assert(IsNaNIEEEFP(bit_width, high_sign, high_expo, high_mant) == false);  

  if (high_fp == 0) {
    if (include_denormal) 
      return 1 + NFPSBetweenTwoIEEEFPS(bit_width, 
				       low_fp, 
				       MakeIEEEFP(bit_width, 1, 0, 1), // the largest -denormal
				       true);
    else 
      return 1 + NFPSBetweenTwoIEEEFPS(bit_width, 
				       low_fp, 
				       MakeIEEEFP(bit_width, 1, 1, 0), // the largest -normal
				       false);
  }
  else if (low_fp == 0) {
    if (include_denormal) 
      return 1 + NFPSBetweenTwoIEEEFPS(bit_width, 
				       MakeIEEEFP(bit_width, 0, 0, 1), // the smallest +denormal
				       high_fp, 
				       true);
    else 
      return 1 + NFPSBetweenTwoIEEEFPS(bit_width, 
				       MakeIEEEFP(bit_width, 0, 1, 0), // the smallest +normal
				       high_fp, 
				       false); 
  }
  else if (high_sign == 0 && low_sign == 1) {
    return NFPSBetweenTwoIEEEFPS(bit_width, 0, high_fp, include_denormal) 
      + NFPSBetweenTwoIEEEFPS(bit_width, low_fp, 0, include_denormal);
  }
  else if (high_sign == 0 && low_sign == 0) {
    bool subn_high = IsDenormalIEEEFP(high_sign, high_expo, high_mant); 
    bool subn_low = IsDenormalIEEEFP(low_sign, low_expo, low_mant); 

    if ((subn_high == true) &&
	(subn_low == true)) {
      assert(high_mant >= low_mant);
      return high_mant - low_mant; 
    }
    else if ((subn_high == false) &&
	     (subn_low == true)) {
      n_fps = NFPSBetweenTwoIEEEFPS(bit_width, 
				    MakeIEEEFP(bit_width, 0, 1, 0), // the smallest +normal
				    high_fp, 
				    include_denormal); 
      if (include_denormal) { 
	__float128 new_high_fp; // the largest +denormal
	uint128_t mask_ones = MaxUint128(); 
	switch (bit_width) {
	case 32:
	  new_high_fp = MakeIEEEFP(bit_width, 0, 0, 0x7FFFFF);
	  break;
	case 64:
	  new_high_fp = MakeIEEEFP(bit_width, 0, 0, 0xFFFFFFFFFFFFF);
	  break;
	case 128:
	  mask_ones = mask_ones >> 16; 
	  new_high_fp = MakeIEEEFP(bit_width, 0, 0, mask_ones);
	  break;
	default:
	  assert(false && "ERROR: Impossible Case... \n");
	  break;
	}
	n_fps += (1 + NFPSBetweenTwoIEEEFPS(bit_width, 
					    low_fp, 
					    new_high_fp, 
					    include_denormal));
      }
     
      return n_fps;
    }
    else if ((subn_high == false) &&
	     (subn_low == false)) {
      if (high_expo == low_expo) {
	assert(high_mant >= low_mant);
	return (high_mant - low_mant);
      }
      else if (high_expo > low_expo) {
	__float128 new_high_fp; 
	uint128_t mask_ones = MaxUint128();
	switch (bit_width) {
	case 32:
	  n_fps = ((high_expo - low_expo) - 1) << 23; 
	  new_high_fp = MakeIEEEFP(bit_width, 0, low_expo, 0x7FFFFF); 
	  break;
	case 64:
	  n_fps = ((high_expo - low_expo) - 1) << 52; 
	  new_high_fp = MakeIEEEFP(bit_width, 0, low_expo, 0xFFFFFFFFFFFFF);
	  break;
	case 128:
	  n_fps = ((high_expo - low_expo) - 1) << 112; 
	  mask_ones = mask_ones >> 16; 
	  new_high_fp = MakeIEEEFP(bit_width, 0, low_expo, mask_ones);
	  break;
	default:
	  assert(false && "ERROR: Impossible Case... \n");
	  break;
	}
	n_fps += NFPSBetweenTwoIEEEFPS(bit_width, 
				       MakeIEEEFP(bit_width, 0, high_expo, 0), 
				       high_fp, 
				       include_denormal);
	n_fps += (1 + NFPSBetweenTwoIEEEFPS(bit_width, 
					    low_fp, 
					    new_high_fp, 
					    include_denormal));
      }
      else assert(false && "ERROR: Impossible Case... \n");
    }
    else assert(false && "ERROR: Impossible Case... \n");
  }
  else if (high_sign == 1 && low_sign == 1) {
    return NFPSBetweenTwoIEEEFPS(bit_width, 
				 (-1)*high_fp, 
				 (-1)*low_fp, 
				 include_denormal);
  }
  else assert(false && "Error Impossible Case... \n");

  return n_fps; 
}
*/

// ---- shift IEEE floating-point numbers ---- 
// direction == 0 --> to +inf
// direction == 1 --> to -inf
template<typename FPT, typename INTT> 
void ShiftIEEEFP (int sign, INTT expo, INTT mant, 
		  int direction, 
		  INTT distance,
		  bool include_denormal, 
		  int &out_sign, INTT &out_expo, INTT &out_mant) {
  // cout << "---- class (" << sign << ", " << expo << ", " << mant << ") (" << direction << ", " << distance << ") ----" << endl;

  assert(sizeof(float) == 4 && 
	 sizeof(double) == 8 && 
	 sizeof(__float128) == 16); 
  assert(sizeof(FPT) == sizeof(INTT)); 
  assert(sizeof(FPT) == sizeof(float) || 
	 sizeof(FPT) == sizeof(double) ||
	 sizeof(FPT) == sizeof(__float128)); 

  assert(sign == 0 || sign == 1);
  assert(direction == 0 || direction == 1);
  assert(include_denormal || (IsDenormalIEEEFP(sign, expo, mant) == false));

  if (distance == 0) {
    out_sign = sign;
    out_expo = expo;
    out_mant = mant;
    return ;
  }

  int bit_width = sizeof(INTT) * 8; 

  INTT max_mant; 
  INTT diff_expo; 
  INTT diff_mant;
  switch (bit_width) {
  case 32:
    max_mant = 0x7FFFFF; 
    diff_expo = distance >> 23; 
    diff_mant = distance & max_mant; 
    break;
  case 64:
    max_mant = 0xFFFFFFFFFFFFF; 
    diff_expo = distance >> 52;
    diff_mant = distance & max_mant; 
    break;
  case 128:
    max_mant = (MaxUint<INTT>() >> 16); 
    diff_expo = distance >> 112; 
    diff_mant = distance & max_mant; 
    break;
  default:
    assert(false && "ERROR: Impossible Case... \n");
    break;
  }

  if (sign == 0) {
    if (direction == 0) { // go +inf
      if (IsZeroIEEEFP(sign, expo, mant)) { // start from zero
	if (include_denormal) 
	  ShiftIEEEFP<FPT, INTT>(sign, 0, 1, 
				 0, 
				 (distance - 1), 
				 include_denormal, 
				 out_sign, out_expo, out_mant);
	else 
	  ShiftIEEEFP<FPT, INTT>(sign, 1, 0, 
				 0, 
				 (distance - 1), 
				 include_denormal, 
				 out_sign, out_expo, out_mant);
	return ; 
      }
      else { // start from non-zero 
	out_sign = 0;
	out_expo = expo + diff_expo;
	if ((max_mant - mant) < diff_mant) {
	  out_expo++; 
	  diff_mant = (diff_mant - (max_mant - mant)) - 1; 
	  out_mant = diff_mant; 
	}
	else {
	  out_mant = mant + diff_mant; 
	}
	return ;
      }
    }
    else { // go -inf
      if (IsZeroIEEEFP(sign, expo, mant)) { // start from zero 
	ShiftIEEEFP<FPT, INTT>(sign, expo, mant, 
			       0, 
			       distance, 
			       include_denormal, 
			       out_sign, out_expo, out_mant); 
	out_sign = (out_sign == 1 ? 0 : 1);
	return ; 
      }
      else { // start from non-zero
	INTT gap;
	if (include_denormal)
	  gap = NFPSBetweenTwoIEEEFPS<FPT, INTT>(MakeIEEEFP<FPT, INTT>(sign, 0, 1), 
						MakeIEEEFP<FPT, INTT>(sign, expo, mant), 
						include_denormal);
	else 
	  gap = NFPSBetweenTwoIEEEFPS<FPT, INTT>(MakeIEEEFP<FPT, INTT>(sign, 1, 0), 
						MakeIEEEFP<FPT, INTT>(sign, expo, mant), 
						include_denormal);
	  
	if (gap >= distance) {
	  if (include_denormal) 
	    ShiftIEEEFP<FPT, INTT>(sign, 0, 1, 
				   0, 
				   (gap - distance), 
				   include_denormal, 
				   out_sign, out_expo, out_mant); 
	  else 
	    ShiftIEEEFP<FPT, INTT>(sign, 1, 0, 
				   0, 
				   (gap - distance), 
				   include_denormal, 
				   out_sign, out_expo, out_mant); 

	}
	else if ((gap+1) == distance) {
	  DecomposeIEEEFP<FPT, INTT>(0, out_sign, out_expo, out_mant);
	}
	else { // (gap+1) < distance
	  if (include_denormal)
	    ShiftIEEEFP<FPT, INTT>(sign, 0, 1, 
				   0, 
				   ((distance - (gap+1)) - 1), 
				   include_denormal, 
				   out_sign, out_expo, out_mant); 
	  else 
	    ShiftIEEEFP<FPT, INTT>(sign, 1, 0, 
				   0, 
				   ((distance - (gap+1)) - 1), 
				   include_denormal, 
				   out_sign, out_expo, out_mant); 
	  out_sign = (out_sign == 0 ? 1 : 0); 
	}
	
	return ;
      }
    }
  }
  else { // sign == 1
    ShiftIEEEFP<FPT, INTT>(0, expo, mant, 
			   (direction == 0 ? 1 : 0), 
			   distance,
			   include_denormal, 
			   out_sign, out_expo, out_mant); 
    out_sign = (out_sign == 0 ? 1 : 0); 
    
    return ;
  }
}

template<typename FPT, typename INTT> 
void ShiftIEEEFP (FPT in_fp, 
		  int direction, 
		  INTT distance,
		  bool include_denormal, 
		  FPT &out_fp) {
  int in_sign, out_sign;
  INTT in_expo, out_expo;
  INTT in_mant, out_mant;
  DecomposeIEEEFP<FPT, INTT>(in_fp, in_sign, in_expo, in_mant);

  ShiftIEEEFP<FPT, INTT>(in_sign, in_expo, in_mant, 
			 direction, 
			 distance, 
			 include_denormal, 
			 out_sign, out_expo, out_mant);
  out_fp = MakeIEEEFP<FPT, INTT>(out_sign, out_expo, out_mant);
}


/* 
   given integers, return IEEEFP 
*/
template<typename TFP, typename TUI> 
TFP makeIEEEFPfromIntegers (TUI uisign, TUI uimant, TUI uiexpo) {
  int i = 0; 
  assert(sizeof(TFP) == sizeof(TUI)); 
  assert(uisign == 1 || uisign == 0); 

  TUI uiret; 
  if (uisign == 0) uiret = 0; 
  else uiret = 1; // uisign == 1

  switch (sizeof(TFP)) { 
  case 4:
    // make expo. 
    assert((uiexpo >> 8) == 0);
    uiret = (uiret << 8) + uiexpo; 
    // make mant. 
    assert((uimant >> 23) == 0); 
    uiret = (uiret << 23) + uimant;
    break;
  case 8: 
    // make expo. 
    assert((uiexpo >> 11) == 0); 
    uiret = (uiret << 11) + uiexpo; 
    // make mant. 
    assert((uimant >> 52) == 0);
    uiret = (uiret << 52) + uimant; 
    break; 
  case 16: 
    assert(false && "ERROR: Unsupported FP type");
    break; 
  default: 
    assert(false && "ERROR: Unsupported FP type");
    break; 
  }

  TFP fret; 
  memcpy(&fret, &uiret, sizeof(TFP));

  return fret; 
}


/* 
   given strings, return IEEEFP 
*/
template<typename TFP> 
TFP makeIEEEFPfromStrings (string ssign, string smant, string sexpo) {
  assert(ssign.length() == 1); 

  TFP fret; 

  if (sizeof(TFP) == 4) {
    uint32_t uisign = 0, uimant = 0, uiexpo = 0; 
    assert(smant.length() == 23); 
    assert(sexpo.length() == 8); 
    
    if (ssign.compare("0") == 0) 
      uisign = 0; 
    else if (ssign.compare("1") == 0)
      uisign = 1; 
    else assert(false && "ERROR: Invalid Sign String"); 

    for (int i = 0 ; i < 23 ; i++) {
      uimant = uimant << 1;
      switch (smant[i]) {
      case '0': 
	break; 
      case '1':
	uimant += 1; 
	break; 
      default:
	assert(false && "ERROR: Wrong Mant. String");
	break; 
      }
    }

    for (int i = 0 ; i < 8 ; i++) {
      uiexpo = uiexpo << 1; 
      switch (sexpo[i]) {
      case '0':
	break; 
      case '1':
	uiexpo += 1; 
	break; 
      default: 
	assert(false && "ERROR: Wrong Expo. String"); 
	break; 
      }
    } 

    return makeIEEEFPfromIntegers<float, uint32_t>(uisign, uimant, uiexpo); 
  }
  else if (sizeof(TFP) == 8) {
    uint64_t uisign = 0, uimant = 0, uiexpo = 0; 
    assert(smant.length() == 52); 
    assert(sexpo.length() == 11); 

    if (ssign.compare("0") == 0) 
      uisign = 0; 
    else if (ssign.compare("1") == 0)
      uisign = 1; 
    else assert(false && "ERROR: Invalid Sign String"); 

    for (int i = 0 ; i < 52 ; i++) {
      uimant = uimant << 1;
      switch (smant[i]) {
      case '0': 
	break; 
      case '1':
	uimant += 1; 
	break; 
      default:
	assert(false && "ERROR: Wrong Mant. String");
	break; 
      }
    }

    for (int i = 0 ; i < 11 ; i++) {
      uiexpo = uiexpo << 1; 
      switch (sexpo[i]) {
      case '0':
	break; 
      case '1':
	uiexpo += 1; 
	break; 
      default: 
	assert(false && "ERROR: Wrong Expo. String"); 
	break; 
      }
    } 

    return makeIEEEFPfromIntegers<double, uint64_t>(uisign, uimant, uiexpo); 
  }
  else {
    assert(false && "ERROR: Unsupported FP type");
  }
}


#endif // #ifndef S3FP_FP_UTILS 
