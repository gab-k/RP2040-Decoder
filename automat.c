int
fsm_main(const char *s)
{
    const char *p;

    enum {
        S0, S1, S2, S3, S4, S5, S6, S7, S8, S9,
        S10, S11, S12, S13, S14, S15, S16, S17, S18, S19,
        S20, S21, S22, S23, S24, S25, S26, S27, S28, S29,
        S30, S31, S32, S33, S34, S35, S36, S37, S38
    } state;

    state = S0;

    for (p = s; *p != '\0'; p++) {
        switch (state) {
            case S0: /* start */
            switch ((unsigned char) *p) {
                case '1': state = S1; break;
                default:  return -1; /* leaf */
            }
            break;

            case S1: /* e.g. "1" */
            switch ((unsigned char) *p) {
                case '1': state = S2; break;
                default:  return -1; /* leaf */
            }
            break;

            case S2: /* e.g. "11" */
            switch ((unsigned char) *p) {
                case '1': state = S3; break;
                default:  return -1; /* leaf */
            }
            break;

            case S3: /* e.g. "111" */
            switch ((unsigned char) *p) {
                case '1': state = S4; break;
                default:  return -1; /* leaf */
            }
            break;

            case S4: /* e.g. "1111" */
            switch ((unsigned char) *p) {
                case '1': state = S5; break;
                default:  return -1; /* leaf */
            }
            break;

            case S5: /* e.g. "11111" */
            switch ((unsigned char) *p) {
                case '1': state = S6; break;
                default:  return -1; /* leaf */
            }
            break;

            case S6: /* e.g. "111111" */
            switch ((unsigned char) *p) {
                case '1': state = S7; break;
                default:  return -1; /* leaf */
            }
            break;

            case S7: /* e.g. "1111111" */
            switch ((unsigned char) *p) {
                case '1': state = S8; break;
                default:  return -1; /* leaf */
            }
            break;

            case S8: /* e.g. "11111111" */
            switch ((unsigned char) *p) {
                case '1': state = S9; break;
                default:  return -1; /* leaf */
            }
            break;

            case S9: /* e.g. "111111111" */
            switch ((unsigned char) *p) {
                case '1': state = S10; break;
                default:  return -1; /* leaf */
            }
            break;

            case S10: /* e.g. "1111111111" */
            switch ((unsigned char) *p) {
                case '1': break;
                case '0': state = S11; break;
                default:  return -1; /* leaf */
            }
            break;

            case S11: /* e.g. "11111111110" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S12; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S12: /* e.g. "111111111100" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S13; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S13: /* e.g. "1111111111000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S14; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S14: /* e.g. "11111111110000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S15; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S15: /* e.g. "111111111100000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S16; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S16: /* e.g. "1111111111000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S17; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S17: /* e.g. "11111111110000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S18; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S18: /* e.g. "111111111100000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S19; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S19: /* e.g. "1111111111000000000" */
            switch ((unsigned char) *p) {
                case '0': state = S20; break;
                default:  return -1; /* leaf */
            }
            break;

            case S20: /* e.g. "11111111110000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S21; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S21: /* e.g. "111111111100000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S22; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S22: /* e.g. "1111111111000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S23; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S23: /* e.g. "11111111110000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S24; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S24: /* e.g. "111111111100000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S25; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S25: /* e.g. "1111111111000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S26; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S26: /* e.g. "11111111110000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S27; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S27: /* e.g. "111111111100000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S28; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S28: /* e.g. "1111111111000000000000000000" */
            switch ((unsigned char) *p) {
                case '0': state = S29; break;
                default:  return -1; /* leaf */
            }
            break;

            case S29: /* e.g. "11111111110000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S30; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S30: /* e.g. "111111111100000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S31; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S31: /* e.g. "1111111111000000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S32; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S32: /* e.g. "11111111110000000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S33; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S33: /* e.g. "111111111100000000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S34; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S34: /* e.g. "1111111111000000000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S35; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S35: /* e.g. "11111111110000000000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S36; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S36: /* e.g. "111111111100000000000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S37; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S37: /* e.g. "1111111111000000000000000000000000000" */
            switch ((unsigned char) *p) {
                case '0':
                    case '1': state = S38; break;
                    default:  return -1; /* leaf */
            }
            break;

            case S38: /* e.g. "11111111110000000000000000000000000000" */
            return -1; /* leaf */

            default:
                ; /* unreached */
        }
    }

    /* end states */
    switch (state) {
        case S38: return 0x1; /* "1*1{10}0[01]{8}0[01]{8}0[01]{8}[01]" */
        default: return -1; /* unexpected EOT */
    }
}


