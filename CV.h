#define CV_1 5      //Address                   -   Basic Address 
#define CV_2 0      //V_min                     -   Minimum Speed
#define CV_3 5      //Beschleunigungsrate       -   Definiert die Länge der Wartezeit, die beim Beschleunigen jeweils vor dem Hochschalten zur nächst höheren Fahrstufe vergeht.
#define CV_4 5      //Bremsrate                 -   Definiert die Länge der Wartezeit, die beim Bremsen jeweils vor dem Herunterschalten zur nächst niedrigen Fahrstufe vergeht.
#define CV_5 255    //V_max                     -   Maximum Speed
#define CV_6 127    //V_mid                     -   Middle Speed
#define CV_7 13     //Version No.               -   Decoder-Software version
#define CV_8 13     //Manufacturer              -   Manufacturer Identification (13 == "Public Domain & Do-It-Yourself Decoders").
#define CV_9 5      //PWM-Period                -   Defines the Motor PWM-Period
const uint8_t CV_FUNCTION_ARRAY [256] = {
//F0 forward
    0b11111111,         //byte 0    -   CV_257
    0b11111111,         //byte 1    -   CV_258
    0b11111111,         //byte 2    -   CV_259
    0b11111111,         //byte 3    -   CV_260
//F0 reverse
    0b11111111,         //byte 0    -   CV_261
    0b11111111,         //byte 1    -   CV_262
    0b11111111,         //byte 2    -   CV_263
    0b11111111,         //byte 3    -   CV_264
//F1 forward
    0b11111111,         //byte 0    -   CV_265
    0b11111111,         //byte 1    -   CV_266
    0b11111111,         //byte 2    -   CV_267
    0b11111111,         //byte 3    -   CV_268
//F1 reverse
    0b11111111,         //byte 0    -   CV_269
    0b11111111,         //byte 1    -   CV_270
    0b11111111,         //byte 2    -   CV_271
    0b11111111,         //byte 3    -   CV_272
//F2 forward
    0b11111111,         //byte 0    -   CV_273
    0b11111111,         //byte 1    -   CV_274
    0b11111111,         //byte 2    -   CV_275
    0b11111111,         //byte 3    -   CV_276
//F2 reverse
    0b11111111,         //byte 0    -   CV_277
    0b11111111,         //byte 1    -   CV_278
    0b11111111,         //byte 2    -   CV_279
    0b11111111,         //byte 3    -   CV_280
//F3 forward
    0b11111111,         //byte 0    -   CV_281
    0b11111111,         //byte 1    -   CV_282
    0b11111111,         //byte 2    -   CV_283
    0b11111111,         //byte 3    -   CV_284
//F3 reverse
    0b11111111,         //byte 0    -   CV_285
    0b11111111,         //byte 1    -   CV_286
    0b11111111,         //byte 2    -   CV_287
    0b11111111,         //byte 3    -   CV_288
//F4 forward
    0b11111111,         //byte 0    -   CV_289
    0b11111111,         //byte 1    -   CV_290
    0b11111111,         //byte 2    -   CV_291
    0b11111111,         //byte 3    -   CV_292
//F4 reverse
    0b11111111,         //byte 0    -   CV_293
    0b11111111,         //byte 1    -   CV_294
    0b11111111,         //byte 2    -   CV_295
    0b11111111,         //byte 3    -   CV_296
//F5 forward
    0b11111111,         //byte 0    -   CV_297
    0b11111111,         //byte 1    -   CV_298
    0b11111111,         //byte 2    -   CV_299
    0b11111111,         //byte 3    -   CV_300
//F5 reverse
    0b11111111,         //byte 0    -   CV_301
    0b11111111,         //byte 1    -   CV_302
    0b11111111,         //byte 2    -   CV_303
    0b11111111,         //byte 3    -   CV_304
//F6 forward
    0b11111111,         //byte 0    -   CV_305
    0b11111111,         //byte 1    -   CV_306
    0b11111111,         //byte 2    -   CV_307
    0b11111111,         //byte 3    -   CV_308
//F6 reverse
    0b11111111,         //byte 0    -   CV_309
    0b11111111,         //byte 1    -   CV_310
    0b11111111,         //byte 2    -   CV_311
    0b11111111,         //byte 3    -   CV_312
//F7 forward
    0b11111111,         //byte 0    -   CV_313
    0b11111111,         //byte 1    -   CV_314
    0b11111111,         //byte 2    -   CV_315
    0b11111111,         //byte 3    -   CV_316
//F7 reverse
    0b11111111,         //byte 0    -   CV_317
    0b11111111,         //byte 1    -   CV_318
    0b11111111,         //byte 2    -   CV_319
    0b11111111,         //byte 3    -   CV_320
//F8 forward
    0b11111111,         //byte 0    -   CV_321
    0b11111111,         //byte 1    -   CV_322
    0b11111111,         //byte 2    -   CV_323
    0b11111111,         //byte 3    -   CV_324
//F8 reverse
    0b11111111,         //byte 0    -   CV_325
    0b11111111,         //byte 1    -   CV_326
    0b11111111,         //byte 2    -   CV_327
    0b11111111,         //byte 3    -   CV_328
//F9 forward
    0b11111111,         //byte 0    -   CV_329
    0b11111111,         //byte 1    -   CV_330
    0b11111111,         //byte 2    -   CV_331
    0b11111111,         //byte 3    -   CV_332
//F9 reverse
    0b11111111,         //byte 0    -   CV_333
    0b11111111,         //byte 1    -   CV_334
    0b11111111,         //byte 2    -   CV_335
    0b11111111,         //byte 3    -   CV_336
//F10 forward
    0b11111111,         //byte 0    -   CV_337
    0b11111111,         //byte 1    -   CV_338
    0b11111111,         //byte 2    -   CV_339
    0b11111111,         //byte 3    -   CV_340
//F10 reverse
    0b11111111,         //byte 0    -   CV_341
    0b11111111,         //byte 1    -   CV_342
    0b11111111,         //byte 2    -   CV_343
    0b11111111,         //byte 3    -   CV_344
//F11 forward
    0b11111111,         //byte 0    -   CV_345
    0b11111111,         //byte 1    -   CV_346
    0b11111111,         //byte 2    -   CV_347
    0b11111111,         //byte 3    -   CV_348
//F11 reverse
    0b11111111,         //byte 0    -   CV_349
    0b11111111,         //byte 1    -   CV_350
    0b11111111,         //byte 2    -   CV_351
    0b11111111,         //byte 3    -   CV_352
//F12 forward
    0b11111111,         //byte 0    -   CV_353
    0b11111111,         //byte 1    -   CV_354
    0b11111111,         //byte 2    -   CV_355
    0b11111111,         //byte 3    -   CV_356
//F12 reverse
    0b11111111,         //byte 0    -   CV_357
    0b11111111,         //byte 1    -   CV_358
    0b11111111,         //byte 2    -   CV_359
    0b11111111,         //byte 3    -   CV_360
//F13 forward
    0b11111111,         //byte 0    -   CV_361
    0b11111111,         //byte 1    -   CV_362
    0b11111111,         //byte 2    -   CV_363
    0b11111111,         //byte 3    -   CV_364
//F13 reverse
    0b11111111,         //byte 0    -   CV_365
    0b11111111,         //byte 1    -   CV_366
    0b11111111,         //byte 2    -   CV_367
    0b11111111,         //byte 3    -   CV_368
//F14 forward
    0b11111111,         //byte 0    -   CV_369
    0b11111111,         //byte 1    -   CV_370
    0b11111111,         //byte 2    -   CV_371
    0b11111111,         //byte 3    -   CV_372
//F14 reverse
    0b11111111,         //byte 0    -   CV_373
    0b11111111,         //byte 1    -   CV_374
    0b11111111,         //byte 2    -   CV_375
    0b11111111,         //byte 3    -   CV_376
//F15 forward
    0b11111111,         //byte 0    -   CV_377
    0b11111111,         //byte 1    -   CV_378
    0b11111111,         //byte 2    -   CV_379
    0b11111111,         //byte 3    -   CV_380
//F15 reverse
    0b11111111,         //byte 0    -   CV_381
    0b11111111,         //byte 1    -   CV_382
    0b11111111,         //byte 2    -   CV_383
    0b11111111,         //byte 3    -   CV_384
//F16 forward
    0b11111111,         //byte 0    -   CV_385
    0b11111111,         //byte 1    -   CV_386
    0b11111111,         //byte 2    -   CV_387
    0b11111111,         //byte 3    -   CV_388
//F16 reverse
    0b11111111,         //byte 0    -   CV_389
    0b11111111,         //byte 1    -   CV_390
    0b11111111,         //byte 2    -   CV_391
    0b11111111,         //byte 3    -   CV_392
//F17 forward
    0b11111111,         //byte 0    -   CV_393
    0b11111111,         //byte 1    -   CV_394
    0b11111111,         //byte 2    -   CV_395
    0b11111111,         //byte 3    -   CV_396
//F17 reverse
    0b11111111,         //byte 0    -   CV_397
    0b11111111,         //byte 1    -   CV_398
    0b11111111,         //byte 2    -   CV_399
    0b11111111,         //byte 3    -   CV_400
//F18 forward
    0b11111111,         //byte 0    -   CV_401
    0b11111111,         //byte 1    -   CV_402
    0b11111111,         //byte 2    -   CV_403
    0b11111111,         //byte 3    -   CV_404
//F18 reverse
    0b11111111,         //byte 0    -   CV_405
    0b11111111,         //byte 1    -   CV_406
    0b11111111,         //byte 2    -   CV_407
    0b11111111,         //byte 3    -   CV_408
//F19 forward
    0b11111111,         //byte 0    -   CV_409
    0b11111111,         //byte 1    -   CV_410
    0b11111111,         //byte 2    -   CV_411
    0b11111111,         //byte 3    -   CV_412
//F19 reverse
    0b11111111,         //byte 0    -   CV_413
    0b11111111,         //byte 1    -   CV_414
    0b11111111,         //byte 2    -   CV_415
    0b11111111,         //byte 3    -   CV_416
//F20 forward
    0b11111111,         //byte 0    -   CV_417
    0b11111111,         //byte 1    -   CV_418
    0b11111111,         //byte 2    -   CV_419
    0b11111111,         //byte 3    -   CV_420
//F20 reverse
    0b11111111,         //byte 0    -   CV_421
    0b11111111,         //byte 1    -   CV_422
    0b11111111,         //byte 2    -   CV_423
    0b11111111,         //byte 3    -   CV_424
//F21 forward
    0b11111111,         //byte 0    -   CV_425
    0b11111111,         //byte 1    -   CV_426
    0b11111111,         //byte 2    -   CV_427
    0b11111111,         //byte 3    -   CV_428
//F21 reverse
    0b11111111,         //byte 0    -   CV_429
    0b11111111,         //byte 1    -   CV_430
    0b11111111,         //byte 2    -   CV_431
    0b11111111,         //byte 3    -   CV_432
//F22 forward
    0b11111111,         //byte 0    -   CV_433
    0b11111111,         //byte 1    -   CV_434
    0b11111111,         //byte 2    -   CV_435
    0b11111111,         //byte 3    -   CV_436
//F22 reverse
    0b11111111,         //byte 0    -   CV_437
    0b11111111,         //byte 1    -   CV_438
    0b11111111,         //byte 2    -   CV_439
    0b11111111,         //byte 3    -   CV_440
//F23 forward
    0b11111111,         //byte 0    -   CV_441
    0b11111111,         //byte 1    -   CV_442
    0b11111111,         //byte 2    -   CV_443
    0b11111111,         //byte 3    -   CV_444
//F23 reverse
    0b11111111,         //byte 0    -   CV_445
    0b11111111,         //byte 1    -   CV_446
    0b11111111,         //byte 2    -   CV_447
    0b11111111,         //byte 3    -   CV_448
//F24 forward
    0b11111111,         //byte 0    -   CV_449
    0b11111111,         //byte 1    -   CV_450
    0b11111111,         //byte 2    -   CV_451
    0b11111111,         //byte 3    -   CV_452
//F24 reverse
    0b11111111,         //byte 0    -   CV_453
    0b11111111,         //byte 1    -   CV_454
    0b11111111,         //byte 2    -   CV_455
    0b11111111,         //byte 3    -   CV_456
//F25 forward
    0b11111111,         //byte 0    -   CV_457
    0b11111111,         //byte 1    -   CV_458
    0b11111111,         //byte 2    -   CV_459
    0b11111111,         //byte 3    -   CV_460
//F25 reverse
    0b11111111,         //byte 0    -   CV_461
    0b11111111,         //byte 1    -   CV_462
    0b11111111,         //byte 2    -   CV_463
    0b11111111,         //byte 3    -   CV_464
//F26 forward
    0b11111111,         //byte 0    -   CV_465
    0b11111111,         //byte 1    -   CV_466
    0b11111111,         //byte 2    -   CV_467
    0b11111111,         //byte 3    -   CV_468
//F26 reverse
    0b11111111,         //byte 0    -   CV_469
    0b11111111,         //byte 1    -   CV_470
    0b11111111,         //byte 2    -   CV_471
    0b11111111,         //byte 3    -   CV_472
//F27 forward
    0b11111111,         //byte 0    -   CV_473
    0b11111111,         //byte 1    -   CV_474
    0b11111111,         //byte 2    -   CV_475
    0b11111111,         //byte 3    -   CV_476
//F27 reverse
    0b11111111,         //byte 0    -   CV_477
    0b11111111,         //byte 1    -   CV_478
    0b11111111,         //byte 2    -   CV_479
    0b11111111,         //byte 3    -   CV_480
//F28 forward
    0b11111111,         //byte 0    -   CV_481
    0b11111111,         //byte 1    -   CV_482
    0b11111111,         //byte 2    -   CV_483
    0b11111111,         //byte 3    -   CV_484
//F28 reverse
    0b11111111,         //byte 0    -   CV_485
    0b11111111,         //byte 1    -   CV_486
    0b11111111,         //byte 2    -   CV_487
    0b11111111,         //byte 3    -   CV_488
//F29 forward
    0b11111111,         //byte 0    -   CV_489
    0b11111111,         //byte 1    -   CV_490
    0b11111111,         //byte 2    -   CV_491
    0b11111111,         //byte 3    -   CV_492
//F29 reverse
    0b11111111,         //byte 0    -   CV_493
    0b11111111,         //byte 1    -   CV_494
    0b11111111,         //byte 2    -   CV_495
    0b11111111,         //byte 3    -   CV_496
//F30 forward
    0b11111111,         //byte 0    -   CV_497
    0b11111111,         //byte 1    -   CV_498
    0b11111111,         //byte 2    -   CV_499
    0b11111111,         //byte 3    -   CV_500
//F30 reverse
    0b11111111,         //byte 0    -   CV_501
    0b11111111,         //byte 1    -   CV_502
    0b11111111,         //byte 2    -   CV_503
    0b11111111,         //byte 3    -   CV_504
//F31 forward
    0b11111111,         //byte 0    -   CV_505
    0b11111111,         //byte 1    -   CV_506
    0b11111111,         //byte 2    -   CV_507
    0b11111111,         //byte 3    -   CV_508
//F31 reverse
    0b11111111,         //byte 0    -   CV_509
    0b11111111,         //byte 1    -   CV_510
    0b11111111,         //byte 2    -   CV_511
    0b11111111,         //byte 3    -   CV_512
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
