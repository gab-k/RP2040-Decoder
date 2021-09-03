//All CVs are 8-bit numbers (range 0 - 255)
#define CV_1 5      //Address                   -   Basic Address
#define CV_2 1      //V_min                     -   Minimum Speed
#define CV_3 40    //Acceleration Rate         -   Acceleration formula: (CV_3*0.896)/(number of speed steps in use)   -> CV_3*7ms/Speed Step
#define CV_4 40     //Deceleration Rate         -   Deceleration formula: (CV_4*0.896)/(number of speed steps in use)   -> CV_4*7ms/Speed Step
#define CV_5 255    //V_max                     -   Maximum Speed
#define CV_6 127    //V_mid                     -   Middle Speed
#define CV_7 13     //Version No.               -   Decoder-Software version
#define CV_8 13     //Manufacturer              -   Manufacturer Identification (13 == "Public Domain & Do-It-Yourself Decoders").
//  PWM Period Frequency Calculation: pwm_period = ( CV_9*CV_5 / CV_10 ) * 8ns
//  The reciprocal of the period is the PWM frequency
#define CV_9 255    //PWM-Period                -   Defines the Motor PWM-Period
#define CV_10 1     //PWM Clock Divider         -   Additional Clock Divider for PWM-Signal
const uint8_t CV_FUNCTION_ARRAY [256] = {
//  31 30 29 28 - 27 26 25 24 -- 23 22 21 20 - 19 18 17 16 -- 15 14 13 12 - 11 10 9  8 -- 7 6 5 4 - 3 2 1 0   <-  GPIO
//           byte_3           --           byte_2          --           byte_1         --       byte_0
//  X  X  X  X  - X  X  X  X  -- X  X  X  X  - X  X  X  X  -- X  X  X  X  - X  X  X  X -- X X X X - X X X X
//F0 forward
    0b00000000,         //byte 0    -   CV_257 - Array Index: 0
    0b00000000,         //byte 1    -   CV_258 - Array Index: 1
    0b00010010,         //byte 2    -   CV_259 - Array Index: 2
    0b00000000,         //byte 3    -   CV_260 - Array Index: 3
//F0 reverse
    0b00000000,         //byte 0    -   CV_261 - Array Index: 4
    0b00000000,         //byte 1    -   CV_262 - Array Index: 5
    0b00001100,         //byte 2    -   CV_263 - Array Index: 6
    0b00000000,         //byte 3    -   CV_264 - Array Index: 7
//F1 forward
    0b00000010,         //byte 0    -   CV_265 - Array Index: 8
    0b00000000,         //byte 1    -   CV_266 - Array Index: 9
    0b00000000,         //byte 2    -   CV_267 - Array Index: 10
    0b00000000,         //byte 3    -   CV_268 - Array Index: 11
//F1 reverse
    0b00000010,         //byte 0    -   CV_269 - Array Index: 12
    0b00000000,         //byte 1    -   CV_270 - Array Index: 13
    0b00000000,         //byte 2    -   CV_271 - Array Index: 14
    0b00000000,         //byte 3    -   CV_272 - Array Index: 15
//F2 forward
    0b00000100,         //byte 0    -   CV_273 - Array Index: 16
    0b00000000,         //byte 1    -   CV_274 - Array Index: 17
    0b00000000,         //byte 2    -   CV_275 - Array Index: 18
    0b00000000,         //byte 3    -   CV_276 - Array Index: 19
//F2 reverse
    0b00000100,         //byte 0    -   CV_277 - Array Index: 20
    0b00000000,         //byte 1    -   CV_278 - Array Index: 21
    0b00000000,         //byte 2    -   CV_279 - Array Index: 22
    0b00000000,         //byte 3    -   CV_280 - Array Index: 23
//F3 forward
    0b00001000,         //byte 0    -   CV_281 - Array Index: 24
    0b00000000,         //byte 1    -   CV_282 - Array Index: 25
    0b00000000,         //byte 2    -   CV_283 - Array Index: 26
    0b00000000,         //byte 3    -   CV_284 - Array Index: 27
//F3 reverse
    0b00001000,         //byte 0    -   CV_285 - Array Index: 28
    0b00000000,         //byte 1    -   CV_286 - Array Index: 29
    0b00000000,         //byte 2    -   CV_287 - Array Index: 30
    0b00000000,         //byte 3    -   CV_288 - Array Index: 31
//F4 forward
    0b00010000,         //byte 0    -   CV_289 - Array Index: 32
    0b00000000,         //byte 1    -   CV_290 - Array Index: 33
    0b00000000,         //byte 2    -   CV_291 - Array Index: 34
    0b00000000,         //byte 3    -   CV_292 - Array Index: 35
//F4 reverse
    0b00010000,         //byte 0    -   CV_293 - Array Index: 36
    0b00000000,         //byte 1    -   CV_294 - Array Index: 37
    0b00000000,         //byte 2    -   CV_295 - Array Index: 38
    0b00000000,         //byte 3    -   CV_296 - Array Index: 39
//F5 forward
    0b00100000,         //byte 0    -   CV_297 - Array Index: 40
    0b00000000,         //byte 1    -   CV_298 - Array Index: 41
    0b00000000,         //byte 2    -   CV_299 - Array Index: 42
    0b00000000,         //byte 3    -   CV_300 - Array Index: 43
//F5 reverse
    0b00100000,         //byte 0    -   CV_301 - Array Index: 44
    0b00000000,         //byte 1    -   CV_302 - Array Index: 45
    0b00000000,         //byte 2    -   CV_303 - Array Index: 46
    0b00000000,         //byte 3    -   CV_304 - Array Index: 47
//F6 forward
    0b01000000,         //byte 0    -   CV_305 - Array Index: 48
    0b00000000,         //byte 1    -   CV_306 - Array Index: 49
    0b00000000,         //byte 2    -   CV_307 - Array Index: 50
    0b00000000,         //byte 3    -   CV_308 - Array Index: 51
//F6 reverse
    0b01000000,         //byte 0    -   CV_309 - Array Index: 52
    0b00000000,         //byte 1    -   CV_310 - Array Index: 53
    0b00000000,         //byte 2    -   CV_311 - Array Index: 54
    0b00000000,         //byte 3    -   CV_312 - Array Index: 55
//F7 forward
    0b10000000,         //byte 0    -   CV_313 - Array Index: 56
    0b00000000,         //byte 1    -   CV_314 - Array Index: 57
    0b00000000,         //byte 2    -   CV_315 - Array Index: 58
    0b00000000,         //byte 3    -   CV_316 - Array Index: 59
//F7 reverse
    0b10000000,         //byte 0    -   CV_317 - Array Index: 60
    0b00000000,         //byte 1    -   CV_318 - Array Index: 61
    0b00000000,         //byte 2    -   CV_319 - Array Index: 62
    0b00000000,         //byte 3    -   CV_320 - Array Index: 63
//F8 forward
    0b00000000,         //byte 0    -   CV_321 - Array Index: 64
    0b00000001,         //byte 1    -   CV_322 - Array Index: 65
    0b00000000,         //byte 2    -   CV_323 - Array Index: 66
    0b00000000,         //byte 3    -   CV_324 - Array Index: 67
//F8 reverse
    0b00000000,         //byte 0    -   CV_325 - Array Index: 68
    0b00000001,         //byte 1    -   CV_326 - Array Index: 69
    0b00000000,         //byte 2    -   CV_327 - Array Index: 70
    0b00000000,         //byte 3    -   CV_328 - Array Index: 71
//F9 forward
    0b00000000,         //byte 0    -   CV_329 - Array Index: 72
    0b00000010,         //byte 1    -   CV_330 - Array Index: 73
    0b00000000,         //byte 2    -   CV_331 - Array Index: 74
    0b00000000,         //byte 3    -   CV_332 - Array Index: 75
//F9 reverse
    0b00000000,         //byte 0    -   CV_333 - Array Index: 76
    0b00000010,         //byte 1    -   CV_334 - Array Index: 77
    0b00000000,         //byte 2    -   CV_335 - Array Index: 78
    0b00000000,         //byte 3    -   CV_336 - Array Index: 79
//F10 forward
    0b00000000,         //byte 0    -   CV_337 - Array Index: 80
    0b00000100,         //byte 1    -   CV_338 - Array Index: 81
    0b00000000,         //byte 2    -   CV_339 - Array Index: 82
    0b00000000,         //byte 3    -   CV_340 - Array Index: 83
//F10 reverse
    0b00000000,         //byte 0    -   CV_341 - Array Index: 84
    0b00000100,         //byte 1    -   CV_342 - Array Index: 85
    0b00000000,         //byte 2    -   CV_343 - Array Index: 86
    0b00000000,         //byte 3    -   CV_344 - Array Index: 87
//F11 forward
    0b00000000,         //byte 0    -   CV_345 - Array Index: 88
    0b00001000,         //byte 1    -   CV_346 - Array Index: 89
    0b00000000,         //byte 2    -   CV_347 - Array Index: 90
    0b00000000,         //byte 3    -   CV_348 - Array Index: 91
//F11 reverse
    0b00000000,         //byte 0    -   CV_349 - Array Index: 92
    0b00001000,         //byte 1    -   CV_350 - Array Index: 93
    0b00000000,         //byte 2    -   CV_351 - Array Index: 94
    0b00000000,         //byte 3    -   CV_352 - Array Index: 95
//F12 forward
    0b00000000,         //byte 0    -   CV_353 - Array Index: 96
    0b00010000,         //byte 1    -   CV_354 - Array Index: 97
    0b00000000,         //byte 2    -   CV_355 - Array Index: 98
    0b00000000,         //byte 3    -   CV_356 - Array Index: 99
//F12 reverse
    0b00000000,         //byte 0    -   CV_357 - Array Index: 100
    0b00010000,         //byte 1    -   CV_358 - Array Index: 101
    0b00000000,         //byte 2    -   CV_359 - Array Index: 102
    0b00000000,         //byte 3    -   CV_360 - Array Index: 103
//F13 forward
    0b00000000,         //byte 0    -   CV_361 - Array Index: 104
    0b00100000,         //byte 1    -   CV_362 - Array Index: 105
    0b00000000,         //byte 2    -   CV_363 - Array Index: 106
    0b00000000,         //byte 3    -   CV_364 - Array Index: 107
//F13 reverse
    0b00000000,         //byte 0    -   CV_365 - Array Index: 108
    0b00100000,         //byte 1    -   CV_366 - Array Index: 109
    0b00000000,         //byte 2    -   CV_367 - Array Index: 110
    0b00000000,         //byte 3    -   CV_368 - Array Index: 111
//F14 forward
    0b00000000,         //byte 0    -   CV_369 - Array Index: 112
    0b01000000,         //byte 1    -   CV_370 - Array Index: 113
    0b00000000,         //byte 2    -   CV_371 - Array Index: 114
    0b00000000,         //byte 3    -   CV_372 - Array Index: 115
//F14 reverse
    0b00000000,         //byte 0    -   CV_373 - Array Index: 116
    0b01000000,         //byte 1    -   CV_374 - Array Index: 117
    0b00000000,         //byte 2    -   CV_375 - Array Index: 118
    0b00000000,         //byte 3    -   CV_376 - Array Index: 119
//F15 forward
    0b00000000,         //byte 0    -   CV_377 - Array Index: 120
    0b10000000,         //byte 1    -   CV_378 - Array Index: 121
    0b00000000,         //byte 2    -   CV_379 - Array Index: 122
    0b00000000,         //byte 3    -   CV_380 - Array Index: 123
//F15 reverse
    0b00000000,         //byte 0    -   CV_381 - Array Index: 124
    0b10000000,         //byte 1    -   CV_382 - Array Index: 125
    0b00000000,         //byte 2    -   CV_383 - Array Index: 126
    0b00000000,         //byte 3    -   CV_384 - Array Index: 127
//F16 forward
    0b00000000,         //byte 0    -   CV_385 - Array Index: 128
    0b00000000,         //byte 1    -   CV_386 - Array Index: 129
    0b00000001,         //byte 2    -   CV_387 - Array Index: 130
    0b00000000,         //byte 3    -   CV_388 - Array Index: 131
//F16 reverse
    0b00000000,         //byte 0    -   CV_389 - Array Index: 132
    0b00000000,         //byte 1    -   CV_390 - Array Index: 133
    0b00000001,         //byte 2    -   CV_391 - Array Index: 134
    0b00000000,         //byte 3    -   CV_392 - Array Index: 135
//F17 forward
    0b00000000,         //byte 0    -   CV_393 - Array Index: 136
    0b00000000,         //byte 1    -   CV_394 - Array Index: 137
    0b00000000,         //byte 2    -   CV_395 - Array Index: 138
    0b00000000,         //byte 3    -   CV_396 - Array Index: 139
//F17 reverse
    0b00000000,         //byte 0    -   CV_397 - Array Index: 140
    0b00000000,         //byte 1    -   CV_398 - Array Index: 141
    0b00000000,         //byte 2    -   CV_399 - Array Index: 142
    0b00000000,         //byte 3    -   CV_400 - Array Index: 143
//F18 forward
    0b00000000,         //byte 0    -   CV_401 - Array Index: 144
    0b00000000,         //byte 1    -   CV_402 - Array Index: 145
    0b00000000,         //byte 2    -   CV_403 - Array Index: 146
    0b00000000,         //byte 3    -   CV_404 - Array Index: 147
//F18 reverse
    0b00000000,         //byte 0    -   CV_405 - Array Index: 148
    0b00000000,         //byte 1    -   CV_406 - Array Index: 149
    0b00000000,         //byte 2    -   CV_407 - Array Index: 150
    0b00000000,         //byte 3    -   CV_408 - Array Index: 151
//F19 forward
    0b00000000,         //byte 0    -   CV_409 - Array Index: 152
    0b00000000,         //byte 1    -   CV_410 - Array Index: 153
    0b00000000,         //byte 2    -   CV_411 - Array Index: 154
    0b00000000,         //byte 3    -   CV_412 - Array Index: 155
//F19 reverse
    0b00000000,         //byte 0    -   CV_413 - Array Index: 156
    0b00000000,         //byte 1    -   CV_414 - Array Index: 157
    0b00000000,         //byte 2    -   CV_415 - Array Index: 158
    0b00000000,         //byte 3    -   CV_416 - Array Index: 159
//F20 forward
    0b00000000,         //byte 0    -   CV_417 - Array Index: 160
    0b00000000,         //byte 1    -   CV_418 - Array Index: 161
    0b00000000,         //byte 2    -   CV_419 - Array Index: 162
    0b00000000,         //byte 3    -   CV_420 - Array Index: 163
//F20 reverse
    0b00000000,         //byte 0    -   CV_421 - Array Index: 164
    0b00000000,         //byte 1    -   CV_422 - Array Index: 165
    0b00000000,         //byte 2    -   CV_423 - Array Index: 166
    0b00000000,         //byte 3    -   CV_424 - Array Index: 167
//F21 forward
    0b00000000,         //byte 0    -   CV_425 - Array Index: 168
    0b00000000,         //byte 1    -   CV_426 - Array Index: 169
    0b00000000,         //byte 2    -   CV_427 - Array Index: 170
    0b00000000,         //byte 3    -   CV_428 - Array Index: 171
//F21 reverse
    0b00000000,         //byte 0    -   CV_429 - Array Index: 172
    0b00000000,         //byte 1    -   CV_430 - Array Index: 173
    0b00000000,         //byte 2    -   CV_431 - Array Index: 174
    0b00000000,         //byte 3    -   CV_432 - Array Index: 175
//F22 forward
    0b00000000,         //byte 0    -   CV_433 - Array Index: 176
    0b00000000,         //byte 1    -   CV_434 - Array Index: 177
    0b00000000,         //byte 2    -   CV_435 - Array Index: 178
    0b00000000,         //byte 3    -   CV_436 - Array Index: 179
//F22 reverse
    0b00000000,         //byte 0    -   CV_437 - Array Index: 180
    0b00000000,         //byte 1    -   CV_438 - Array Index: 181
    0b00000000,         //byte 2    -   CV_439 - Array Index: 182
    0b00000000,         //byte 3    -   CV_440 - Array Index: 183
//F23 forward
    0b00000000,         //byte 0    -   CV_441 - Array Index: 184
    0b00000000,         //byte 1    -   CV_442 - Array Index: 185
    0b00000000,         //byte 2    -   CV_443 - Array Index: 186
    0b00000000,         //byte 3    -   CV_444 - Array Index: 187
//F23 reverse
    0b00000000,         //byte 0    -   CV_445 - Array Index: 188
    0b00000000,         //byte 1    -   CV_446 - Array Index: 189
    0b00000000,         //byte 2    -   CV_447 - Array Index: 190
    0b00000000,         //byte 3    -   CV_448 - Array Index: 191
//F24 forward
    0b00000000,         //byte 0    -   CV_449 - Array Index: 192
    0b00000000,         //byte 1    -   CV_450 - Array Index: 193
    0b00000000,         //byte 2    -   CV_451 - Array Index: 194
    0b00000000,         //byte 3    -   CV_452 - Array Index: 195
//F24 reverse
    0b00000000,         //byte 0    -   CV_453 - Array Index: 196
    0b00000000,         //byte 1    -   CV_454 - Array Index: 197
    0b00000000,         //byte 2    -   CV_455 - Array Index: 198
    0b00000000,         //byte 3    -   CV_456 - Array Index: 199
//F25 forward
    0b00000000,         //byte 0    -   CV_457 - Array Index: 200
    0b00000000,         //byte 1    -   CV_458 - Array Index: 201
    0b00000000,         //byte 2    -   CV_459 - Array Index: 202
    0b00000000,         //byte 3    -   CV_460 - Array Index: 203
//F25 reverse
    0b00000000,         //byte 0    -   CV_461 - Array Index: 204
    0b00000000,         //byte 1    -   CV_462 - Array Index: 205
    0b00000000,         //byte 2    -   CV_463 - Array Index: 206
    0b00000000,         //byte 3    -   CV_464 - Array Index: 207
//F26 forward
    0b00000000,         //byte 0    -   CV_465 - Array Index: 208
    0b00000000,         //byte 1    -   CV_466 - Array Index: 209
    0b00000000,         //byte 2    -   CV_467 - Array Index: 210
    0b00000000,         //byte 3    -   CV_468 - Array Index: 211
//F26 reverse
    0b00000000,         //byte 0    -   CV_469 - Array Index: 212
    0b00000000,         //byte 1    -   CV_470 - Array Index: 213
    0b00000000,         //byte 2    -   CV_471 - Array Index: 214
    0b00000000,         //byte 3    -   CV_472 - Array Index: 215
//F27 forward
    0b00000000,         //byte 0    -   CV_473 - Array Index: 216
    0b00000000,         //byte 1    -   CV_474 - Array Index: 217
    0b00000000,         //byte 2    -   CV_475 - Array Index: 218
    0b00000000,         //byte 3    -   CV_476 - Array Index: 219
//F27 reverse
    0b00000000,         //byte 0    -   CV_477 - Array Index: 220
    0b00000000,         //byte 1    -   CV_478 - Array Index: 221
    0b00000000,         //byte 2    -   CV_479 - Array Index: 222
    0b00000000,         //byte 3    -   CV_480 - Array Index: 223
//F28 forward
    0b00000000,         //byte 0    -   CV_481 - Array Index: 224
    0b00000000,         //byte 1    -   CV_482 - Array Index: 225
    0b00000000,         //byte 2    -   CV_483 - Array Index: 226
    0b00010000,         //byte 3    -   CV_484 - Array Index: 227
//F28 reverse
    0b00000000,         //byte 0    -   CV_485 - Array Index: 228
    0b00000000,         //byte 1    -   CV_486 - Array Index: 229
    0b00000000,         //byte 2    -   CV_487 - Array Index: 230
    0b00010000,         //byte 3    -   CV_488 - Array Index: 231
//F29 forward
    0b00000000,         //byte 0    -   CV_489 - Array Index: 232
    0b00000000,         //byte 1    -   CV_490 - Array Index: 233
    0b00000000,         //byte 2    -   CV_491 - Array Index: 234
    0b00000000,         //byte 3    -   CV_492 - Array Index: 235
//F29 reverse
    0b00000000,         //byte 0    -   CV_493 - Array Index: 236
    0b00000000,         //byte 1    -   CV_494 - Array Index: 237
    0b00000000,         //byte 2    -   CV_495 - Array Index: 238
    0b00000000,         //byte 3    -   CV_496 - Array Index: 239
//F30 forward
    0b00000000,         //byte 0    -   CV_497 - Array Index: 240
    0b00000000,         //byte 1    -   CV_498 - Array Index: 241
    0b00000000,         //byte 2    -   CV_499 - Array Index: 242
    0b00000000,         //byte 3    -   CV_500 - Array Index: 243
//F30 reverse
    0b00000000,         //byte 0    -   CV_501 - Array Index: 244
    0b00000000,         //byte 1    -   CV_502 - Array Index: 245
    0b00000000,         //byte 2    -   CV_503 - Array Index: 246
    0b00000000,         //byte 3    -   CV_504 - Array Index: 247
//F31 forward
    0b00000000,         //byte 0    -   CV_505 - Array Index: 248
    0b00000000,         //byte 1    -   CV_506 - Array Index: 249
    0b00000000,         //byte 2    -   CV_507 - Array Index: 250
    0b00000000,         //byte 3    -   CV_508 - Array Index: 251
//F31 reverse
    0b00000000,         //byte 0    -   CV_509 - Array Index: 252
    0b00000000,         //byte 1    -   CV_510 - Array Index: 253
    0b00000000,         //byte 2    -   CV_511 - Array Index: 254
    0b00000000,         //byte 3    -   CV_512 - Array Index: 255
};
// #define CV_33 0b00000001        //F0 (Licht) vorwärts   (MSB) A8 - A1 (LSB)
// #define CV_34 0b00000010        //F0 (Licht) rückwärts  (MSB) A8 - A1 (LSB)
// #define CV_35 0b00000100        //F1                    (MSB) A8 - A1 (LSB)
// #define CV_36 0b00001000        //F2                    (MSB) A8 - A1 (LSB)
// #define CV_37 0b00010000        //F3                    (MSB) A8 - A1 (LSB)
// #define CV_38 0b00000100        //F4                    (MSB) A11 - A4 (LSB)
// #define CV_39 0b00001000        //F5                    (MSB) A11 - A4 (LSB)
// #define CV_40 0b00010000        //F6                    (MSB) A11 - A4 (LSB)
// #define CV_41 0b00100000        //F7                    (MSB) A11 - A4 (LSB)
// #define CV_42 0b01000000        //F8                    (MSB) A11 - A4 (LSB)
// #define CV_43 0b00010000        //F9                    (MSB) A14 - A9 (LSB)
// #define CV_44 0b00100000        //F10                   (MSB) A14 - A9 (LSB)
// #define CV_45 0b01000000        //F11                   (MSB) A14 - A9 (LSB)
// #define CV_46 0b10000000        //F12                   (MSB) A14 - A9 (LSB)
