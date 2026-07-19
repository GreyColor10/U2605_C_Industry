#pragma once

#define CheckTrue(x) do { if(x == true) return; } while(0)
#define CheckTrueResult(x, y) do { if(x == true) return y; } while(0)

#define CheckFalse(x) do { if(x == false) return; } while(0)
#define CheckFalseResult(x, y) do { if(x == false) return y; } while(0)

#define CheckNull(x) do { if(x == nullptr) return; } while(0)
#define CheckNullResult(x, y) do { if(x == nullptr) return y; } while(0)

#define CheckNotValid(x) do { if(!IsValid(x)) return; } while(0)
#define CheckNotValidResult(x, y) do { if(!IsValid(x)) return y; } while(0)