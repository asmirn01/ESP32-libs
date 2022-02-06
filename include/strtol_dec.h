#include <limits.h>
#include <ctype.h>

long
strtol_dec(const char *nptr, char **endptr)
{
  register unsigned long acc;
  register unsigned char c;
  register unsigned long cutoff;
  register signed char any;
  unsigned char flag = 0;
#define FL_NEG  0x01    /* number is negative */

  if (endptr)
    *endptr = (char *)nptr;

  /*
     Skip white space and pick up leading +/- sign if any.
     If base is 0, allow 0x for hex and 0 for octal, else
     assume decimal; if base is already 16, allow 0x.
  */
  do {
    c = *nptr++;
  } while (isspace(c));

  if (c == '-') {
    flag = FL_NEG;
    c = *nptr++;
  }

  /*
     Compute the cutoff value between legal numbers and illegal
     numbers.  That is the largest legal value, divided by the
     base.  An input number that is greater than this value, if
     followed by a legal input character, is too big.  One that
     is equal to this value may be valid or not; the decision
     about this is done as outlined below.

     Overflow detections works as follows:

     As:
        acc_old <= cutoff
     then:
        acc_old * base <= 0x80000000        (unsigned)
     then:
        acc_old * base + c <= 0x80000000 + c
     or:
        acc_new <= 0x80000000 + 35

     i.e. carry from MSB (by calculating acc_new) is impossible
     and we can check result directly:

        if (acc_new > 0x80000000) then overflow

     Set any if any `digits' consumed; make it negative to indicate
     overflow.
  */
  cutoff = ((unsigned long)LONG_MAX + 1) / 10;

  for (acc = 0, any = 0;; c = *nptr++) {
    if (c >= '0' && c <= '9')
      c -= '0';
    else
      break;
    if (any < 0)
      continue;
    if (acc > cutoff) {
      any = -1;
      continue;
    }
    acc = acc * 10 + c;
    if (acc > (unsigned long)LONG_MAX + 1) {
      any = -1;
    } else {
      any = 1;
    }
  }

  if (endptr && any) {
    *endptr = (char *)nptr - 1;
  }

  if (any < 0) {
    acc = (flag & FL_NEG) ? LONG_MIN : LONG_MAX;
  } else if (flag & FL_NEG) {
    acc = -acc;
  } else if ((signed long)acc < 0) {
    acc = LONG_MAX;
  }
  return (acc);
}
