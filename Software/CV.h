#pragma once
#include "pico/stdlib.h"

#define CV_ARRAY_SIZE 1024
// All CVs are 8-bit numbers or bytes ranging from (0 - 255)dec = (0b00000000 - 0b11111111)bin = (0x00 - 0xFF)hex
// IMPORTANT NOTE : CV_1 @ array_index = 0; CV_2 @ array_index = 1; CV_3 @ array_index = 2; ...; 
// This effectively means: array_index = cv_index - 1

uint8_t CV_ARRAY_DEFAULT [CV_ARRAY_SIZE] = {
   3,                //CV_1  -    Basic address
   0,                //CV_2  -    V_start
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // 0 == Fastest dec/acc rate; 255 == slowest;
   0,                //CV_3  -    Acceleration rate
   0,                //CV_4  -    Deceleration rate
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   255,              //CV_5  -    V_max
   0,                //CV_6  -    V_mid
   2,                //CV_7  -    Version no.                                                         (read-only)
    // Writing a value of 8 to CV_8 resets all CV values to the default values in CV.h
   13,               //CV_8  -    Manufacturer (13 = Public Domain & Do-It-Yourself Decoders)         (read-only)
   0,                //CV_9  -    PWM frequency
   0,                //CV_10 -    BEMF Cutout
   0,                //CV_11 -    Packet timeout
   0,                //CV_12 -    Power Source Conversion
   0,                //CV_13 -    DC Mode Function Status F1-F8
   0,                //CV_14 -    DC Mode Function Status F0 & F9-F12
   0,                //CV_15 -    Decoder Lock (Key)
   0,                //CV_16 -    Decoder Lock (Lock)
   0b11000011,       //CV_17 -    Extended/Long 14-Bit address    (High Byte)
   0b11101000,       //CV_18 -    Extended/Long 14-Bit address    (Low Byte)
   0,                //CV_19 -    Consist Address
   0,                //CV_20 -    (Reserved by NMRA)
   0,                //CV_21 -    Consist Function Control (F1-F8)
   0,                //CV_22 -    Consist Function Control (F0 & F9-F12)
   0,                //CV_23 -    Consist Acceleration Adjustment
   0,                //CV_24 -    Consist Deceleration Adjustment
   0,                //CV_25 -    Speed Table Selection
   0,                //CV_26 -    (Reserved by NMRA)
   0,                //CV_27 -    Decoder Automatic Stopping Configuration
   1,                //CV_28 -    RailCom Configuration - Default: RailCom enabled
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   //Bit_1, Bit_2, Bit_3, Bit_4, Bit_6 are currently not in use and therefore irrelevant.
   //Bit_0 is used to reverse direction i.e. 0 = normal; 1 = reverse
   //Bit_5 switches between basic and extended address i.e. 0 = basic address 1 = extended address
   6,                //CV_29 -    Decoder Configuration - Default: Analog Mode enabled, Normal Direction, Speed Steps 28/128
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0,                //CV_30 -    Error Information
   0b00010000,         //CV_31  -   Indexed CV High Byte
   0b00000000,         //CV_32  -   Indexed CV Low Byte
   1,                //CV_33 -    F0(f) Output Location
   2,                //CV_34 -    F0(r) Output Location
   4,                //CV_35 -    F1 Output Location
   8,                //CV_36 -    F2 Output Location
   16,               //CV_37 -    F3 Output Location
   32,               //CV_38 -    F4 Output Location
   64,               //CV_39 -    F5 Output Location
   128,              //CV_40 -    F6 Output Location
   1,                //CV_41 -    F7 Output Location
   2,                //CV_42 -    F8 Output Location
   4,                //CV_43 -    F9 Output Location
   8,                //CV_44 -    F10 Output Location
   16,               //CV_45 -    F11 Output Location
   32,               //CV_46 -    F12 Output Location
   // Speed controller - Configuration /////////////////////////////////////////////////////////////////////////////////
   0b11100110,         //CV_47  -   Feed-forward factor k_ff in % = CV_47/255  Default = 230 -> 0.902 = 90.2%         //
   0b00001010,         //CV_48  -   PID Control low pass filter time constant (tau) in ms                             //
   0b00000101,         //CV_49  -   PID Control sampling time t in ms                                                 //
   0b00011001,         //CV_50  -   PID Control I_Factor        =   CV_50/10        Default = 25 -> 2.5               //
   0b00110010,         //CV_51  -   PID Control D_Factor        =   CV_51/10000     Default = 50 -> 0.005             //
   0b01100100,         //CV_52  -   PID Integral Limit positive =   CV_52*10        Default = 100 -> +1000            //
   0b01100100,         //CV_53  -   PID Integral Limit negative =   CV_53*(-10)     Default = 100 -> -1000            //
   0b00000111,         //CV_54  -   k_p @ x0 - High Byte (Bits 8-15)                divided by 100                    //
   0b11010000,         //CV_55  -   k_p @ x0 - Low Byte (Bits 0-7)                  Default = 2000 -> 20              //
   0b00000000,         //CV_56  -   k_p @ x1 - High Byte (Bits 8-15)                divided by 100                    //
   0b11111010,         //CV_57  -   k_p @ x1 - Low Byte (Bits 0-7)                  Default = 250 -> 2.5              //
   0b00000000,         //CV_58  -   k_p @ x2 - High Byte (Bits 8-15)                divided by 100                    //
   0b10010110,         //CV_59  -   k_p @ x2 - Low Byte (Bits 0-7)                  Default = 150 -> 1.5              //
   0b00001101,         //CV_60  -   x_1 shift in % = CV_60/255                                                        //
   // V_EMF Measurement Configuration //////////////////////////////////////////////////////////////////////////////////
   0b01100100,         //CV_61  -   V_EMF Total amount of Measurement Iterations    -   Default = 100 iterations      //
   0b01100100,         //CV_62  -   V_EMF Delay before Measuring                    -   Default = 100us               //
   // CV_63 and CV_64 are used to determine how many elements of the sorted measurement array will be discarded       //
   // For Example: CV_63 = x and CV_64 = y                                                                            //
   // -> The x smallest values will be discarded.                                                                     //
   // -> The y largest values will be discarded.                                                                      //
   0b00001111,         //CV_63  -   l_side_arr_cutoff      -   Default = 15                                           //
   0b00001111,         //CV_64  -   r_side_arr_cutoff      -   Default = 15                                           //
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b00000000,         //CV_65  -   Values > 0 indicate that flash is still in factory condition
   0b00000000,         //CV_66  -
   0b00000000,         //CV_67  -
   0b00000000,         //CV_68  -
   0b00000000,         //CV_69  -
   0b00000000,         //CV_70  -
   0b00000000,         //CV_71  -
   0b00000000,         //CV_72  -
   0b00000000,         //CV_73  -
   0b00000000,         //CV_74  -
   0b00000000,         //CV_75  -
   0b00000000,         //CV_76  -
   0b00000000,         //CV_77  -
   0b00000000,         //CV_78  -
   0b00000000,         //CV_79  -
   0b00000000,         //CV_80  -
   0b00000000,         //CV_81  -
   0b00000000,         //CV_82  -
   0b00000000,         //CV_83  -
   0b00000000,         //CV_84  -
   0b00000000,         //CV_85  -
   0b00000000,         //CV_86  -
   0b00000000,         //CV_87  -
   0b00000000,         //CV_88  -
   0b00000000,         //CV_89  -
   0b00000000,         //CV_90  -
   0b00000000,         //CV_91  -
   0b00000000,         //CV_92  -
   0b00000000,         //CV_93  -
   0b00000000,         //CV_94  -
   0b00000000,         //CV_95  -
   0b00000000,         //CV_96  -
   0b00000000,         //CV_97  -
   0b00000000,         //CV_98  -
   0b00000000,         //CV_99  -
   0b00000000,         //CV_100  -
   0b00000000,         //CV_101  -
   0b00000000,         //CV_102  -
   0b00000000,         //CV_103  -
   0b00000000,         //CV_104  -
   0b00000000,         //CV_105  -
   0b00000000,         //CV_106  -
   0b00000000,         //CV_107  -
   0b00000000,         //CV_108  -
   0b00000000,         //CV_109  -
   0b00000000,         //CV_110  -
   0b00000000,         //CV_111  -
///// PWM - Configuration //////////////////////////////////////////////////////////////////////////////////////////////
// This Config can cause conflicts when GPIOs controlled by the same slice are enabled at the same time.
   0b00000000,         //CV_112  -  GPIO 24-31  enable PWM
   0b00000000,         //CV_113  -  GPIO 16-23  enable PWM
   0b00000000,         //CV_114  -  GPIO 8-15   enable PWM
   0b00000000,         //CV_115  -  GPIO 0-7    enable PWM
//Slice 0
   0b00000000,         //CV_116  -  Wrap Bits 8-15
   0b00000000,         //CV_117  -  Wrap Bits 0-7
   0b00000000,         //CV_118  -  Clock Divider
   0b00000000,         //CV_119  -  Channel A Level Bits 8-15
   0b00000000,         //CV_120  -  Channel A Level Bits 0-7
   0b00000000,         //CV_121  -  Channel B Level Bits 8-15
   0b00000000,         //CV_122  -  Channel B Level Bits 0-7
//Slice 1
   0b00000000,         //CV_123  -  Wrap Bits 8-15
   0b00000000,         //CV_124  -  Wrap Bits 0-7
   0b00000000,         //CV_125  -  Clock Divider
   0b00000000,         //CV_126  -  Channel A Level Bits 8-15
   0b00000000,         //CV_127  -  Channel A Level Bits 0-7
   0b00000000,         //CV_128  -  Channel B Level Bits 8-15
   0b00000000,         //CV_129  -  Channel B Level Bits 0-7
//Slice 2
   0b00000000,         //CV_130  -  Wrap Bits 8-15
   0b00000000,         //CV_131  -  Wrap Bits 0-7
   0b00000000,         //CV_132  -  Clock Divider
   0b00000000,         //CV_133  -  Channel A Level Bits 8-15
   0b00000000,         //CV_134  -  Channel A Level Bits 0-7
   0b00000000,         //CV_135  -  Channel B Level Bits 8-15
   0b00000000,         //CV_136  -  Channel B Level Bits 0-7
//Slice 3
   0b00000000,         //CV_137  -  Wrap Bits 8-15
   0b00000000,         //CV_138  -  Wrap Bits 0-7
   0b00000000,         //CV_139  -  Clock Divider
   0b00000000,         //CV_140  -  Channel A Level Bits 8-15
   0b00000000,         //CV_141  -  Channel A Level Bits 0-7
   0b00000000,         //CV_142  -  Channel B Level Bits 8-15
   0b00000000,         //CV_143  -  Channel B Level Bits 0-7
//Slice 4
   0b00000000,         //CV_144  -  Wrap Bits 8-15
   0b00000000,         //CV_145  -  Wrap Bits 0-7
   0b00000000,         //CV_146  -  Clock Divider
   0b00000000,         //CV_147  -  Channel A Level Bits 8-15
   0b00000000,         //CV_148  -  Channel A Level Bits 0-7
   0b00000000,         //CV_149  -  Channel B Level Bits 8-15
   0b00000000,         //CV_150  -  Channel B Level Bits 0-7
//Slice 5
   0b00000000,         //CV_151  -  Wrap Bits 8-15
   0b00000000,         //CV_152  -  Wrap Bits 0-7
   0b00000000,         //CV_153  -  Clock Divider
   0b00000000,         //CV_154  -  Channel A Level Bits 8-15
   0b00000000,         //CV_155  -  Channel A Level Bits 0-7
   0b00000000,         //CV_156  -  Channel B Level Bits 8-15
   0b00000000,         //CV_157  -  Channel B Level Bits 0-7
//Slice 6
   0b00000000,         //CV_158  -  Wrap Bits 8-15
   0b00000000,         //CV_159  -  Wrap Bits 0-7
   0b00000000,         //CV_160  -  Clock Divider
   0b00000000,         //CV_161  -  Channel A Level Bits 8-15
   0b00000000,         //CV_162  -  Channel A Level Bits 0-7
   0b00000000,         //CV_163  -  Channel B Level Bits 8-15
   0b00000000,         //CV_164  -  Channel B Level Bits 0-7
//Slice 7
   0b00000000,         //CV_165  -  Wrap Bits 8-15
   0b00000000,         //CV_166  -  Wrap Bits 0-7
   0b00000000,         //CV_167  -  Clock Divider
   0b00000000,         //CV_168  -  Channel A Level Bits 8-15
   0b00000000,         //CV_169  -  Channel A Level Bits 0-7
   0b00000000,         //CV_170  -  Channel B Level Bits 8-15
   0b00000000,         //CV_171  -  Channel B Level Bits 0-7
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Offset adjustment for ADC                                                                                           //
//If CV_172 = 0xFF = 255 there will be a measurement procedure on startup                                             //
   0b11111111,         //CV_172  -  ADC offset                              Default: 255  (triggers new measurement!) //
   0b01111111,         //CV_173  -  ADC offset measurement cycles           Default: 127                              //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b00000001,         //CV_174  -  Additional motor-PWM clock divider.
   0b00000111,         //CV_175  -  speed_helper timer delay -  can be used to adjust accel/decel rate
   0b00000000,         //CV_176  -  
   0b00000000,         //CV_177  -  
   0b00000000,         //CV_178  -  
   0b00000000,         //CV_179  -  
   0b00000000,         //CV_180  -
   0b00000000,         //CV_181  -
   0b00000000,         //CV_182  -
   0b00000000,         //CV_183  -
   0b00000000,         //CV_184  -
   0b00000000,         //CV_185  -
   0b00000000,         //CV_186  -
   0b00000000,         //CV_187  -
   0b00000000,         //CV_188  -
   0b00000000,         //CV_189  -
   0b00000000,         //CV_190  -
   0b00000000,         //CV_191  -
   0b00000000,         //CV_192  -
   0b00000000,         //CV_193  -
   0b00000000,         //CV_194  -
   0b00000000,         //CV_195  -
   0b00000000,         //CV_196  -
   0b00000000,         //CV_197  -
   0b00000000,         //CV_198  -
   0b00000000,         //CV_199  -
   0b00000000,         //CV_200  -
   0b00000000,         //CV_201  -
   0b00000000,         //CV_202  -
   0b00000000,         //CV_203  -
   0b00000000,         //CV_204  -
   0b00000000,         //CV_205  -
   0b00000000,         //CV_206  -
   0b00000000,         //CV_207  -
   0b00000000,         //CV_208  -
   0b00000000,         //CV_209  -
   0b00000000,         //CV_210  -
   0b00000000,         //CV_211  -
   0b00000000,         //CV_212  -
   0b00000000,         //CV_213  -
   0b00000000,         //CV_214  -
   0b00000000,         //CV_215  -
   0b00000000,         //CV_216  -
   0b00000000,         //CV_217  -
   0b00000000,         //CV_218  -
   0b00000000,         //CV_219  -
   0b00000000,         //CV_220  -
   0b00000000,         //CV_221  -
   0b00000000,         //CV_222  -
   0b00000000,         //CV_223  -
   0b00000000,         //CV_224  -
   0b00000000,         //CV_225  -
   0b00000000,         //CV_226  -
   0b00000000,         //CV_227  -
   0b00000000,         //CV_228  -
   0b00000000,         //CV_229  -
   0b00000000,         //CV_230  -
   0b00000000,         //CV_231  -
   0b00000000,         //CV_232  -
   0b00000000,         //CV_233  -
   0b00000000,         //CV_234  -
   0b00000000,         //CV_235  -
   0b00000000,         //CV_236  -
   0b00000000,         //CV_237  -
   0b00000000,         //CV_238  -
   0b00000000,         //CV_239  -
   0b00000000,         //CV_240  -
   0b00000000,         //CV_241  -
   0b00000000,         //CV_242  -
   0b00000000,         //CV_243  -
   0b00000000,         //CV_244  -
   0b00000000,         //CV_245  -
   0b00000000,         //CV_246  -
   0b00000000,         //CV_247  -
   0b00000000,         //CV_248  -
   0b00000000,         //CV_249  -
   0b00000000,         //CV_250  -
   0b00000000,         //CV_251  -
   0b00000000,         //CV_252  -
   0b00000000,         //CV_253  -
   0b00000000,         //CV_254  -
   0b00000000,         //CV_255  -
   0b00000000,         //CV_256  -
//FUNCTION MAPPING CVS:
//  31 30 29 28 - 27 26 25 24 -- 23 22 21 20 - 19 18 17 16 -- 15 14 13 12 - 11 10 9  8 -- 7 6 5 4 - 3 2 1 0   <-  GPIO
//           byte_3           --           byte_2          --           byte_1         --       byte_0
//  X  X  X  X  - X  X  X  X  -- X  X  X  X  - X  X  X  X  -- X  X  X  X  - X  X  X  X -- X X X X - X X X X
//F0 forward
   0b00000000,         //CV_257  -  byte 3
   0b00000000,         //CV_258  -  byte 2
   0b00000000,         //CV_259  -  byte 1
   0b00000000,         //CV_260  -  byte 0
//F0 reverse
   0b00000000,         //CV_261  -  byte 3
   0b00000000,         //CV_262  -  byte 2
   0b00000000,         //CV_263  -  byte 1
   0b00000000,         //CV_264  -  byte 0
//F1 forward
   0b00000000,         //CV_265  -  byte 3
   0b00000000,         //CV_266  -  byte 2
   0b00000000,         //CV_267  -  byte 1
   0b00000000,         //CV_268  -  byte 0
//F1 reverse
   0b00000000,         //CV_269  -  byte 3
   0b00000000,         //CV_270  -  byte 2
   0b00000000,         //CV_271  -  byte 1
   0b00000000,         //CV_272  -  byte 0
//F2 forward
   0b00000000,         //CV_273  -  byte 3
   0b00000000,         //CV_274  -  byte 2
   0b00000000,         //CV_275  -  byte 1
   0b00000000,         //CV_276  -  byte 0
//F2 reverse
   0b00000000,         //CV_277  -  byte 3
   0b00000000,         //CV_278  -  byte 2
   0b00000000,         //CV_279  -  byte 1
   0b00000000,         //CV_280  -  byte 0
//F3 forward
   0b00000000,         //CV_281  -  byte 3
   0b00000000,         //CV_282  -  byte 2
   0b00000000,         //CV_283  -  byte 1
   0b00000000,         //CV_284  -  byte 0
//F3 reverse
   0b00000000,         //CV_285  -  byte 3
   0b00000000,         //CV_286  -  byte 2
   0b00000000,         //CV_287  -  byte 1
   0b00000000,         //CV_288  -  byte 0
//F4 forward
   0b00000000,         //CV_289  -  byte 3
   0b00000000,         //CV_290  -  byte 2
   0b00000000,         //CV_291  -  byte 1
   0b00000000,         //CV_292  -  byte 0
//F4 reverse
   0b00000000,         //CV_293  -  byte 3
   0b00000000,         //CV_294  -  byte 2
   0b00000000,         //CV_295  -  byte 1
   0b00000000,         //CV_296  -  byte 0
//F5 forward
   0b00000000,         //CV_297  -  byte 3
   0b00000000,         //CV_298  -  byte 2
   0b00000000,         //CV_299  -  byte 1
   0b00000000,         //CV_300  -  byte 0
//F5 reverse
   0b00000000,         //CV_301  -  byte 3
   0b00000000,         //CV_302  -  byte 2
   0b00000000,         //CV_303  -  byte 1
   0b00000000,         //CV_304  -  byte 0
//F6 forward
   0b00000000,         //CV_305  -  byte 3
   0b00000000,         //CV_306  -  byte 2
   0b00000000,         //CV_307  -  byte 1
   0b00000000,         //CV_308  -  byte 0
//F6 reverse
   0b00000000,         //CV_309  -  byte 3
   0b00000000,         //CV_310  -  byte 2
   0b00000000,         //CV_311  -  byte 1
   0b00000000,         //CV_312  -  byte 0
//F7 forward
   0b00000000,         //CV_313  -  byte 3
   0b00000000,         //CV_314  -  byte 2
   0b00000000,         //CV_315  -  byte 1
   0b00000000,         //CV_316  -  byte 0
//F7 reverse
   0b00000000,         //CV_317  -  byte 3
   0b00000000,         //CV_318  -  byte 2
   0b00000000,         //CV_319  -  byte 1
   0b00000000,         //CV_320  -  byte 0
//F8 forward
   0b00000000,         //CV_321  -  byte 3
   0b00000000,         //CV_322  -  byte 2
   0b00000000,         //CV_323  -  byte 1
   0b00000000,         //CV_324  -  byte 0
//F8 reverse
   0b00000000,         //CV_325  -  byte 3
   0b00000000,         //CV_326  -  byte 2
   0b00000000,         //CV_327  -  byte 1
   0b00000000,         //CV_328  -  byte 0
//F9 forward
   0b00000000,         //CV_329  -  byte 3
   0b00000000,         //CV_330  -  byte 2
   0b00000000,         //CV_331  -  byte 1
   0b00000000,         //CV_332  -  byte 0
//F9 reverse
   0b00000000,         //CV_333  -  byte 3
   0b00000000,         //CV_334  -  byte 2
   0b00000000,         //CV_335  -  byte 1
   0b00000000,         //CV_336  -  byte 0
//F10 forward
   0b00000000,         //CV_337  -  byte 3
   0b00000000,         //CV_338  -  byte 2
   0b00000000,         //CV_339  -  byte 1
   0b00000000,         //CV_340  -  byte 0
//F10 reverse
   0b00000000,         //CV_341  -  byte 3
   0b00000000,         //CV_342  -  byte 2
   0b00000000,         //CV_343  -  byte 1
   0b00000000,         //CV_344  -  byte 0
//F11 forward
   0b00000000,         //CV_345  -  byte 3
   0b00000000,         //CV_346  -  byte 2
   0b00000000,         //CV_347  -  byte 1
   0b00000000,         //CV_348  -  byte 0
//F11 reverse
   0b00000000,         //CV_349  -  byte 3
   0b00000000,         //CV_350  -  byte 2
   0b00000000,         //CV_351  -  byte 1
   0b00000000,         //CV_352  -  byte 0
//F12 forward
   0b00000000,         //CV_353  -  byte 3
   0b00000000,         //CV_354  -  byte 2
   0b00000000,         //CV_355  -  byte 1
   0b00000000,         //CV_356  -  byte 0
//F12 reverse
   0b00000000,         //CV_357  -  byte 3
   0b00000000,         //CV_358  -  byte 2
   0b00000000,         //CV_359  -  byte 1
   0b00000000,         //CV_360  -  byte 0
//F13 forward
   0b00000000,         //CV_361  -  byte 3
   0b00000000,         //CV_362  -  byte 2
   0b00000000,         //CV_363  -  byte 1
   0b00000000,         //CV_364  -  byte 0
//F13 reverse
   0b00000000,         //CV_365  -  byte 3
   0b00000000,         //CV_366  -  byte 2
   0b00000000,         //CV_367  -  byte 1
   0b00000000,         //CV_368  -  byte 0
//F14 forward
   0b00000000,         //CV_369  -  byte 3
   0b00000000,         //CV_370  -  byte 2
   0b00000000,         //CV_371  -  byte 1
   0b00000000,         //CV_372  -  byte 0
//F14 reverse
   0b00000000,         //CV_373  -  byte 3
   0b00000000,         //CV_374  -  byte 2
   0b00000000,         //CV_375  -  byte 1
   0b00000000,         //CV_376  -  byte 0
//F15 forward
   0b00000000,         //CV_377  -  byte 3
   0b00000000,         //CV_378  -  byte 2
   0b00000000,         //CV_379  -  byte 1
   0b00000000,         //CV_380  -  byte 0
//F15 reverse
   0b00000000,         //CV_381  -  byte 3
   0b00000000,         //CV_382  -  byte 2
   0b00000000,         //CV_383  -  byte 1
   0b00000000,         //CV_384  -  byte 0
//F16 forward
   0b00000000,         //CV_385  -  byte 3
   0b00000000,         //CV_386  -  byte 2
   0b00000000,         //CV_387  -  byte 1
   0b00000000,         //CV_388  -  byte 0
//F16 reverse
   0b00000000,         //CV_389  -  byte 3
   0b00000000,         //CV_390  -  byte 2
   0b00000000,         //CV_391  -  byte 1
   0b00000000,         //CV_392  -  byte 0
//F17 forward
   0b00000000,         //CV_393  -  byte 3
   0b00000000,         //CV_394  -  byte 2
   0b00000000,         //CV_395  -  byte 1
   0b00000000,         //CV_396  -  byte 0
//F17 reverse
   0b00000000,         //CV_397  -  byte 3
   0b00000000,         //CV_398  -  byte 2
   0b00000000,         //CV_399  -  byte 1
   0b00000000,         //CV_400  -  byte 0
//F18 forward
   0b00000000,         //CV_401  -  byte 3
   0b00000000,         //CV_402  -  byte 2
   0b00000000,         //CV_403  -  byte 1
   0b00000000,         //CV_404  -  byte 0
//F18 reverse
   0b00000000,         //CV_405  -  byte 3
   0b00000000,         //CV_406  -  byte 2
   0b00000000,         //CV_407  -  byte 1
   0b00000000,         //CV_408  -  byte 0
//F19 forward
   0b00000000,         //CV_409  -  byte 3
   0b00000000,         //CV_410  -  byte 2
   0b00000000,         //CV_411  -  byte 1
   0b00000000,         //CV_412  -  byte 0
//F19 reverse
   0b00000000,         //CV_413  -  byte 3
   0b00000000,         //CV_414  -  byte 2
   0b00000000,         //CV_415  -  byte 1
   0b00000000,         //CV_416  -  byte 0
//F20 forward
   0b00000000,         //CV_417  -  byte 3
   0b00000000,         //CV_418  -  byte 2
   0b00000000,         //CV_419  -  byte 1
   0b00000000,         //CV_420  -  byte 0
//F20 reverse
   0b00000000,         //CV_421  -  byte 3
   0b00000000,         //CV_422  -  byte 2
   0b00000000,         //CV_423  -  byte 1
   0b00000000,         //CV_424  -  byte 0
//F21 forward
   0b00000000,         //CV_425  -  byte 3
   0b00000000,         //CV_426  -  byte 2
   0b00000000,         //CV_427  -  byte 1
   0b00000000,         //CV_428  -  byte 0
//F21 reverse
   0b00000000,         //CV_429  -  byte 3
   0b00000000,         //CV_430  -  byte 2
   0b00000000,         //CV_431  -  byte 1
   0b00000000,         //CV_432  -  byte 0
//F22 forward
   0b00000000,         //CV_433  -  byte 3
   0b00000000,         //CV_434  -  byte 2
   0b00000000,         //CV_435  -  byte 1
   0b00000000,         //CV_436  -  byte 0
//F22 reverse
   0b00000000,         //CV_437  -  byte 3
   0b00000000,         //CV_438  -  byte 2
   0b00000000,         //CV_439  -  byte 1
   0b00000000,         //CV_440  -  byte 0
//F23 forward
   0b00000000,         //CV_441  -  byte 3
   0b00000000,         //CV_442  -  byte 2
   0b00000000,         //CV_443  -  byte 1
   0b00000000,         //CV_444  -  byte 0
//F23 reverse
   0b00000000,         //CV_445  -  byte 3
   0b00000000,         //CV_446  -  byte 2
   0b00000000,         //CV_447  -  byte 1
   0b00000000,         //CV_448  -  byte 0
//F24 forward
   0b00000000,         //CV_449  -  byte 3
   0b00000000,         //CV_450  -  byte 2
   0b00000000,         //CV_451  -  byte 1
   0b00000000,         //CV_452  -  byte 0
//F24 reverse
   0b00000000,         //CV_453  -  byte 3
   0b00000000,         //CV_454  -  byte 2
   0b00000000,         //CV_455  -  byte 1
   0b00000000,         //CV_456  -  byte 0
//F25 forward
   0b00000000,         //CV_457  -  byte 3
   0b00000000,         //CV_458  -  byte 2
   0b00000000,         //CV_459  -  byte 1
   0b00000000,         //CV_460  -  byte 0
//F25 reverse
   0b00000000,         //CV_461  -  byte 3
   0b00000000,         //CV_462  -  byte 2
   0b00000000,         //CV_463  -  byte 1
   0b00000000,         //CV_464  -  byte 0
//F26 forward
   0b00000000,         //CV_465  -  byte 3
   0b00000000,         //CV_466  -  byte 2
   0b00000000,         //CV_467  -  byte 1
   0b00000000,         //CV_468  -  byte 0
//F26 reverse
   0b00000000,         //CV_469  -  byte 3
   0b00000000,         //CV_470  -  byte 2
   0b00000000,         //CV_471  -  byte 1
   0b00000000,         //CV_472  -  byte 0
//F27 forward
   0b00000000,         //CV_473  -  byte 3
   0b00000000,         //CV_474  -  byte 2
   0b00000000,         //CV_475  -  byte 1
   0b00000000,         //CV_476  -  byte 0
//F27 reverse
   0b00000000,         //CV_477  -  byte 3
   0b00000000,         //CV_478  -  byte 2
   0b00000000,         //CV_479  -  byte 1
   0b00000000,         //CV_480  -  byte 0
//F28 forward
   0b00000000,         //CV_481  -  byte 3
   0b00000000,         //CV_482  -  byte 2
   0b00000000,         //CV_483  -  byte 1
   0b00000000,         //CV_484  -  byte 0
//F28 reverse
   0b00000000,         //CV_485  -  byte 3
   0b00000000,         //CV_486  -  byte 2
   0b00000000,         //CV_487  -  byte 1
   0b00000000,         //CV_488  -  byte 0
//F29 forward
   0b00000000,         //CV_489  -  byte 3
   0b00000000,         //CV_490  -  byte 2
   0b00000000,         //CV_491  -  byte 1
   0b00000000,         //CV_492  -  byte 0
//F29 reverse
   0b00000000,         //CV_493  -  byte 3
   0b00000000,         //CV_494  -  byte 2
   0b00000000,         //CV_495  -  byte 1
   0b00000000,         //CV_496  -  byte 0
//F30 forward
   0b00000000,         //CV_497  -  byte 3
   0b00000000,         //CV_498  -  byte 2
   0b00000000,         //CV_499  -  byte 1
   0b00000000,         //CV_500  -  byte 0
//F30 reverse
   0b00000000,         //CV_501  -  byte 3
   0b00000000,         //CV_502  -  byte 2
   0b00000000,         //CV_503  -  byte 1
   0b00000000,         //CV_504  -  byte 0
//F31 forward
   0b00000000,         //CV_505  -  byte 3
   0b00000000,         //CV_506  -  byte 2
   0b00000000,         //CV_507  -  byte 1
   0b00000000,         //CV_508  -  byte 0
//F31 reverse
   0b00000000,         //CV_509  -  byte 3
   0b00000000,         //CV_510  -  byte 2
   0b00000000,         //CV_511  -  byte 1
   0b00000000,         //CV_512  -  byte 0
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   0b00000000,         //CV_513  -    
   0b00000000,         //CV_514  -    
   0b00000000,         //CV_515  -    
   0b00000000,         //CV_516  -    
   0b00000000,         //CV_517  -    
   0b00000000,         //CV_518  -    
   0b00000000,         //CV_519  -    
   0b00000000,         //CV_520  -    
   0b00000000,         //CV_521  -    
   0b00000000,         //CV_522  -    
   0b00000000,         //CV_523  -    
   0b00000000,         //CV_524  -    
   0b00000000,         //CV_525  -    
   0b00000000,         //CV_526  -    
   0b00000000,         //CV_527  -    
   0b00000000,         //CV_528  -    
   0b00000000,         //CV_529  -    
   0b00000000,         //CV_530  -    
   0b00000000,         //CV_531  -    
   0b00000000,         //CV_532  -    
   0b00000000,         //CV_533  -    
   0b00000000,         //CV_534  -    
   0b00000000,         //CV_535  -    
   0b00000000,         //CV_536  -    
   0b00000000,         //CV_537  -    
   0b00000000,         //CV_538  -    
   0b00000000,         //CV_539  -    
   0b00000000,         //CV_540  -    
   0b00000000,         //CV_541  -    
   0b00000000,         //CV_542  -    
   0b00000000,         //CV_543  -    
   0b00000000,         //CV_544  -    
   0b00000000,         //CV_545  -    
   0b00000000,         //CV_546  -    
   0b00000000,         //CV_547  -    
   0b00000000,         //CV_548  -    
   0b00000000,         //CV_549  -    
   0b00000000,         //CV_550  -    
   0b00000000,         //CV_551  -    
   0b00000000,         //CV_552  -    
   0b00000000,         //CV_553  -    
   0b00000000,         //CV_554  -    
   0b00000000,         //CV_555  -    
   0b00000000,         //CV_556  -    
   0b00000000,         //CV_557  -    
   0b00000000,         //CV_558  -    
   0b00000000,         //CV_559  -    
   0b00000000,         //CV_560  -    
   0b00000000,         //CV_561  -    
   0b00000000,         //CV_562  -    
   0b00000000,         //CV_563  -    
   0b00000000,         //CV_564  -    
   0b00000000,         //CV_565  -    
   0b00000000,         //CV_566  -    
   0b00000000,         //CV_567  -    
   0b00000000,         //CV_568  -    
   0b00000000,         //CV_569  -    
   0b00000000,         //CV_570  -    
   0b00000000,         //CV_571  -    
   0b00000000,         //CV_572  -    
   0b00000000,         //CV_573  -    
   0b00000000,         //CV_574  -    
   0b00000000,         //CV_575  -    
   0b00000000,         //CV_576  -    
   0b00000000,         //CV_577  -    
   0b00000000,         //CV_578  -    
   0b00000000,         //CV_579  -    
   0b00000000,         //CV_580  -    
   0b00000000,         //CV_581  -    
   0b00000000,         //CV_582  -    
   0b00000000,         //CV_583  -    
   0b00000000,         //CV_584  -    
   0b00000000,         //CV_585  -    
   0b00000000,         //CV_586  -    
   0b00000000,         //CV_587  -    
   0b00000000,         //CV_588  -    
   0b00000000,         //CV_589  -    
   0b00000000,         //CV_590  -    
   0b00000000,         //CV_591  -    
   0b00000000,         //CV_592  -    
   0b00000000,         //CV_593  -    
   0b00000000,         //CV_594  -    
   0b00000000,         //CV_595  -    
   0b00000000,         //CV_596  -    
   0b00000000,         //CV_597  -    
   0b00000000,         //CV_598  -    
   0b00000000,         //CV_599  -    
   0b00000000,         //CV_600  -    
   0b00000000,         //CV_601  -    
   0b00000000,         //CV_602  -    
   0b00000000,         //CV_603  -    
   0b00000000,         //CV_604  -    
   0b00000000,         //CV_605  -    
   0b00000000,         //CV_606  -    
   0b00000000,         //CV_607  -    
   0b00000000,         //CV_608  -    
   0b00000000,         //CV_609  -    
   0b00000000,         //CV_610  -    
   0b00000000,         //CV_611  -    
   0b00000000,         //CV_612  -    
   0b00000000,         //CV_613  -    
   0b00000000,         //CV_614  -    
   0b00000000,         //CV_615  -    
   0b00000000,         //CV_616  -    
   0b00000000,         //CV_617  -    
   0b00000000,         //CV_618  -    
   0b00000000,         //CV_619  -    
   0b00000000,         //CV_620  -    
   0b00000000,         //CV_621  -    
   0b00000000,         //CV_622  -    
   0b00000000,         //CV_623  -    
   0b00000000,         //CV_624  -    
   0b00000000,         //CV_625  -    
   0b00000000,         //CV_626  -    
   0b00000000,         //CV_627  -    
   0b00000000,         //CV_628  -    
   0b00000000,         //CV_629  -    
   0b00000000,         //CV_630  -    
   0b00000000,         //CV_631  -    
   0b00000000,         //CV_632  -    
   0b00000000,         //CV_633  -    
   0b00000000,         //CV_634  -    
   0b00000000,         //CV_635  -    
   0b00000000,         //CV_636  -    
   0b00000000,         //CV_637  -    
   0b00000000,         //CV_638  -    
   0b00000000,         //CV_639  -    
   0b00000000,         //CV_640  -    
   0b00000000,         //CV_641  -    
   0b00000000,         //CV_642  -    
   0b00000000,         //CV_643  -    
   0b00000000,         //CV_644  -    
   0b00000000,         //CV_645  -    
   0b00000000,         //CV_646  -    
   0b00000000,         //CV_647  -    
   0b00000000,         //CV_648  -    
   0b00000000,         //CV_649  -    
   0b00000000,         //CV_650  -    
   0b00000000,         //CV_651  -    
   0b00000000,         //CV_652  -    
   0b00000000,         //CV_653  -    
   0b00000000,         //CV_654  -    
   0b00000000,         //CV_655  -    
   0b00000000,         //CV_656  -    
   0b00000000,         //CV_657  -    
   0b00000000,         //CV_658  -    
   0b00000000,         //CV_659  -    
   0b00000000,         //CV_660  -    
   0b00000000,         //CV_661  -    
   0b00000000,         //CV_662  -    
   0b00000000,         //CV_663  -    
   0b00000000,         //CV_664  -    
   0b00000000,         //CV_665  -    
   0b00000000,         //CV_666  -    
   0b00000000,         //CV_667  -    
   0b00000000,         //CV_668  -    
   0b00000000,         //CV_669  -    
   0b00000000,         //CV_670  -    
   0b00000000,         //CV_671  -    
   0b00000000,         //CV_672  -    
   0b00000000,         //CV_673  -    
   0b00000000,         //CV_674  -    
   0b00000000,         //CV_675  -    
   0b00000000,         //CV_676  -    
   0b00000000,         //CV_677  -    
   0b00000000,         //CV_678  -    
   0b00000000,         //CV_679  -    
   0b00000000,         //CV_680  -    
   0b00000000,         //CV_681  -    
   0b00000000,         //CV_682  -    
   0b00000000,         //CV_683  -    
   0b00000000,         //CV_684  -    
   0b00000000,         //CV_685  -    
   0b00000000,         //CV_686  -    
   0b00000000,         //CV_687  -    
   0b00000000,         //CV_688  -    
   0b00000000,         //CV_689  -    
   0b00000000,         //CV_690  -    
   0b00000000,         //CV_691  -    
   0b00000000,         //CV_692  -    
   0b00000000,         //CV_693  -    
   0b00000000,         //CV_694  -    
   0b00000000,         //CV_695  -    
   0b00000000,         //CV_696  -    
   0b00000000,         //CV_697  -    
   0b00000000,         //CV_698  -    
   0b00000000,         //CV_699  -    
   0b00000000,         //CV_700  -    
   0b00000000,         //CV_701  -    
   0b00000000,         //CV_702  -    
   0b00000000,         //CV_703  -    
   0b00000000,         //CV_704  -    
   0b00000000,         //CV_705  -    
   0b00000000,         //CV_706  -    
   0b00000000,         //CV_707  -    
   0b00000000,         //CV_708  -    
   0b00000000,         //CV_709  -    
   0b00000000,         //CV_710  -    
   0b00000000,         //CV_711  -    
   0b00000000,         //CV_712  -    
   0b00000000,         //CV_713  -    
   0b00000000,         //CV_714  -    
   0b00000000,         //CV_715  -    
   0b00000000,         //CV_716  -    
   0b00000000,         //CV_717  -    
   0b00000000,         //CV_718  -    
   0b00000000,         //CV_719  -    
   0b00000000,         //CV_720  -    
   0b00000000,         //CV_721  -    
   0b00000000,         //CV_722  -    
   0b00000000,         //CV_723  -    
   0b00000000,         //CV_724  -    
   0b00000000,         //CV_725  -    
   0b00000000,         //CV_726  -    
   0b00000000,         //CV_727  -    
   0b00000000,         //CV_728  -    
   0b00000000,         //CV_729  -    
   0b00000000,         //CV_730  -    
   0b00000000,         //CV_731  -    
   0b00000000,         //CV_732  -    
   0b00000000,         //CV_733  -    
   0b00000000,         //CV_734  -    
   0b00000000,         //CV_735  -    
   0b00000000,         //CV_736  -    
   0b00000000,         //CV_737  -    
   0b00000000,         //CV_738  -    
   0b00000000,         //CV_739  -    
   0b00000000,         //CV_740  -    
   0b00000000,         //CV_741  -    
   0b00000000,         //CV_742  -    
   0b00000000,         //CV_743  -    
   0b00000000,         //CV_744  -    
   0b00000000,         //CV_745  -    
   0b00000000,         //CV_746  -    
   0b00000000,         //CV_747  -    
   0b00000000,         //CV_748  -    
   0b00000000,         //CV_749  -    
   0b00000000,         //CV_750  -    
   0b00000000,         //CV_751  -    
   0b00000000,         //CV_752  -    
   0b00000000,         //CV_753  -    
   0b00000000,         //CV_754  -    
   0b00000000,         //CV_755  -    
   0b00000000,         //CV_756  -    
   0b00000000,         //CV_757  -    
   0b00000000,         //CV_758  -    
   0b00000000,         //CV_759  -    
   0b00000000,         //CV_760  -    
   0b00000000,         //CV_761  -    
   0b00000000,         //CV_762  -    
   0b00000000,         //CV_763  -    
   0b00000000,         //CV_764  -    
   0b00000000,         //CV_765  -    
   0b00000000,         //CV_766  -    
   0b00000000,         //CV_767  -    
   0b00000000,         //CV_768  -    
   0b00000000,         //CV_769  -    
   0b00000000,         //CV_770  -    
   0b00000000,         //CV_771  -    
   0b00000000,         //CV_772  -    
   0b00000000,         //CV_773  -    
   0b00000000,         //CV_774  -    
   0b00000000,         //CV_775  -    
   0b00000000,         //CV_776  -    
   0b00000000,         //CV_777  -    
   0b00000000,         //CV_778  -    
   0b00000000,         //CV_779  -    
   0b00000000,         //CV_780  -    
   0b00000000,         //CV_781  -    
   0b00000000,         //CV_782  -    
   0b00000000,         //CV_783  -    
   0b00000000,         //CV_784  -    
   0b00000000,         //CV_785  -    
   0b00000000,         //CV_786  -    
   0b00000000,         //CV_787  -    
   0b00000000,         //CV_788  -    
   0b00000000,         //CV_789  -    
   0b00000000,         //CV_790  -    
   0b00000000,         //CV_791  -    
   0b00000000,         //CV_792  -    
   0b00000000,         //CV_793  -    
   0b00000000,         //CV_794  -    
   0b00000000,         //CV_795  -    
   0b00000000,         //CV_796  -    
   0b00000000,         //CV_797  -    
   0b00000000,         //CV_798  -    
   0b00000000,         //CV_799  -    
   0b00000000,         //CV_800  -    
   0b00000000,         //CV_801  -    
   0b00000000,         //CV_802  -    
   0b00000000,         //CV_803  -    
   0b00000000,         //CV_804  -    
   0b00000000,         //CV_805  -    
   0b00000000,         //CV_806  -    
   0b00000000,         //CV_807  -    
   0b00000000,         //CV_808  -    
   0b00000000,         //CV_809  -    
   0b00000000,         //CV_810  -    
   0b00000000,         //CV_811  -    
   0b00000000,         //CV_812  -    
   0b00000000,         //CV_813  -    
   0b00000000,         //CV_814  -    
   0b00000000,         //CV_815  -    
   0b00000000,         //CV_816  -    
   0b00000000,         //CV_817  -    
   0b00000000,         //CV_818  -    
   0b00000000,         //CV_819  -    
   0b00000000,         //CV_820  -    
   0b00000000,         //CV_821  -    
   0b00000000,         //CV_822  -    
   0b00000000,         //CV_823  -    
   0b00000000,         //CV_824  -    
   0b00000000,         //CV_825  -    
   0b00000000,         //CV_826  -    
   0b00000000,         //CV_827  -    
   0b00000000,         //CV_828  -    
   0b00000000,         //CV_829  -    
   0b00000000,         //CV_830  -    
   0b00000000,         //CV_831  -    
   0b00000000,         //CV_832  -    
   0b00000000,         //CV_833  -    
   0b00000000,         //CV_834  -    
   0b00000000,         //CV_835  -    
   0b00000000,         //CV_836  -    
   0b00000000,         //CV_837  -    
   0b00000000,         //CV_838  -    
   0b00000000,         //CV_839  -    
   0b00000000,         //CV_840  -    
   0b00000000,         //CV_841  -    
   0b00000000,         //CV_842  -    
   0b00000000,         //CV_843  -    
   0b00000000,         //CV_844  -    
   0b00000000,         //CV_845  -    
   0b00000000,         //CV_846  -    
   0b00000000,         //CV_847  -    
   0b00000000,         //CV_848  -    
   0b00000000,         //CV_849  -    
   0b00000000,         //CV_850  -    
   0b00000000,         //CV_851  -    
   0b00000000,         //CV_852  -    
   0b00000000,         //CV_853  -    
   0b00000000,         //CV_854  -    
   0b00000000,         //CV_855  -    
   0b00000000,         //CV_856  -    
   0b00000000,         //CV_857  -    
   0b00000000,         //CV_858  -    
   0b00000000,         //CV_859  -    
   0b00000000,         //CV_860  -    
   0b00000000,         //CV_861  -    
   0b00000000,         //CV_862  -    
   0b00000000,         //CV_863  -    
   0b00000000,         //CV_864  -    
   0b00000000,         //CV_865  -    
   0b00000000,         //CV_866  -    
   0b00000000,         //CV_867  -    
   0b00000000,         //CV_868  -    
   0b00000000,         //CV_869  -    
   0b00000000,         //CV_870  -    
   0b00000000,         //CV_871  -    
   0b00000000,         //CV_872  -    
   0b00000000,         //CV_873  -    
   0b00000000,         //CV_874  -    
   0b00000000,         //CV_875  -    
   0b00000000,         //CV_876  -    
   0b00000000,         //CV_877  -    
   0b00000000,         //CV_878  -    
   0b00000000,         //CV_879  -    
   0b00000000,         //CV_880  -    
   0b00000000,         //CV_881  -    
   0b00000000,         //CV_882  -    
   0b00000000,         //CV_883  -    
   0b00000000,         //CV_884  -    
   0b00000000,         //CV_885  -    
   0b00000000,         //CV_886  -    
   0b00000000,         //CV_887  -    
   0b00000000,         //CV_888  -    
   0b00000000,         //CV_889  -    
   0b00000000,         //CV_890  -    
   0b00000000,         //CV_891  -    
   0b00000000,         //CV_892  -    
   0b00000000,         //CV_893  -    
   0b00000000,         //CV_894  -    
   0b00000000,         //CV_895  -    
   0b00000000,         //CV_896  -    
   0b00000000,         //CV_897  -    
   0b00000000,         //CV_898  -    
   0b00000000,         //CV_899  -    
   0b00000000,         //CV_900  -    
   0b00000000,         //CV_901  -    
   0b00000000,         //CV_902  -    
   0b00000000,         //CV_903  -    
   0b00000000,         //CV_904  -    
   0b00000000,         //CV_905  -    
   0b00000000,         //CV_906  -    
   0b00000000,         //CV_907  -    
   0b00000000,         //CV_908  -    
   0b00000000,         //CV_909  -    
   0b00000000,         //CV_910  -    
   0b00000000,         //CV_911  -    
   0b00000000,         //CV_912  -    
   0b00000000,         //CV_913  -    
   0b00000000,         //CV_914  -    
   0b00000000,         //CV_915  -    
   0b00000000,         //CV_916  -    
   0b00000000,         //CV_917  -    
   0b00000000,         //CV_918  -    
   0b00000000,         //CV_919  -    
   0b00000000,         //CV_920  -    
   0b00000000,         //CV_921  -    
   0b00000000,         //CV_922  -    
   0b00000000,         //CV_923  -    
   0b00000000,         //CV_924  -    
   0b00000000,         //CV_925  -    
   0b00000000,         //CV_926  -    
   0b00000000,         //CV_927  -    
   0b00000000,         //CV_928  -    
   0b00000000,         //CV_929  -    
   0b00000000,         //CV_930  -    
   0b00000000,         //CV_931  -    
   0b00000000,         //CV_932  -    
   0b00000000,         //CV_933  -    
   0b00000000,         //CV_934  -    
   0b00000000,         //CV_935  -    
   0b00000000,         //CV_936  -    
   0b00000000,         //CV_937  -    
   0b00000000,         //CV_938  -    
   0b00000000,         //CV_939  -    
   0b00000000,         //CV_940  -    
   0b00000000,         //CV_941  -    
   0b00000000,         //CV_942  -    
   0b00000000,         //CV_943  -    
   0b00000000,         //CV_944  -    
   0b00000000,         //CV_945  -    
   0b00000000,         //CV_946  -    
   0b00000000,         //CV_947  -    
   0b00000000,         //CV_948  -    
   0b00000000,         //CV_949  -    
   0b00000000,         //CV_950  -    
   0b00000000,         //CV_951  -    
   0b00000000,         //CV_952  -    
   0b00000000,         //CV_953  -    
   0b00000000,         //CV_954  -    
   0b00000000,         //CV_955  -    
   0b00000000,         //CV_956  -    
   0b00000000,         //CV_957  -    
   0b00000000,         //CV_958  -    
   0b00000000,         //CV_959  -    
   0b00000000,         //CV_960  -    
   0b00000000,         //CV_961  -    
   0b00000000,         //CV_962  -    
   0b00000000,         //CV_963  -    
   0b00000000,         //CV_964  -    
   0b00000000,         //CV_965  -    
   0b00000000,         //CV_966  -    
   0b00000000,         //CV_967  -    
   0b00000000,         //CV_968  -    
   0b00000000,         //CV_969  -    
   0b00000000,         //CV_970  -    
   0b00000000,         //CV_971  -    
   0b00000000,         //CV_972  -    
   0b00000000,         //CV_973  -    
   0b00000000,         //CV_974  -    
   0b00000000,         //CV_975  -    
   0b00000000,         //CV_976  -    
   0b00000000,         //CV_977  -    
   0b00000000,         //CV_978  -    
   0b00000000,         //CV_979  -    
   0b00000000,         //CV_980  -    
   0b00000000,         //CV_981  -    
   0b00000000,         //CV_982  -    
   0b00000000,         //CV_983  -    
   0b00000000,         //CV_984  -    
   0b00000000,         //CV_985  -    
   0b00000000,         //CV_986  -    
   0b00000000,         //CV_987  -    
   0b00000000,         //CV_988  -    
   0b00000000,         //CV_989  -    
   0b00000000,         //CV_990  -    
   0b00000000,         //CV_991  -    
   0b00000000,         //CV_992  -    
   0b00000000,         //CV_993  -    
   0b00000000,         //CV_994  -    
   0b00000000,         //CV_995  -    
   0b00000000,         //CV_996  -    
   0b00000000,         //CV_997  -    
   0b00000000,         //CV_998  -    
   0b00000000,         //CV_999  -    
   0b00000000,         //CV_1000  -    
   0b00000000,         //CV_1001  -    
   0b00000000,         //CV_1002  -    
   0b00000000,         //CV_1003  -    
   0b00000000,         //CV_1004  -    
   0b00000000,         //CV_1005  -    
   0b00000000,         //CV_1006  -    
   0b00000000,         //CV_1007  -    
   0b00000000,         //CV_1008  -    
   0b00000000,         //CV_1009  -    
   0b00000000,         //CV_1010  -    
   0b00000000,         //CV_1011  -    
   0b00000000,         //CV_1012  -    
   0b00000000,         //CV_1013  -    
   0b00000000,         //CV_1014  -    
   0b00000000,         //CV_1015  -    
   0b00000000,         //CV_1016  -    
   0b00000000,         //CV_1017  -    
   0b00000000,         //CV_1018  -    
   0b00000000,         //CV_1019  -    
   0b00000000,         //CV_1020  -    
   0b00000000,         //CV_1021  -    
   0b00000000,         //CV_1022  -    
   0b00000000,         //CV_1023  -    
   0b00000000,         //CV_1024  -    
};


