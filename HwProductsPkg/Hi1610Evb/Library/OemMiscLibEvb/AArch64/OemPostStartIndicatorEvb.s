#include <Uefi.h>
#include <Library/PcdLib.h>

.text
.align 2

GCC_ASM_EXPORT(OemPostStartIndicator)

ASM_PFX(OemPostStartIndicator):

  ldr w0, =1
  ldr w2, PcdGet32(PcdOemGpioLed0)
  lslv w2, w0, w2
  ldr w3, PcdGet32(PcdOemGpioLed1)
  lslv w3, w0, w3

  ldr x0, =0x602e0000      
  ldr w1, [x0, #0x4]
  orr w1, w1, w2
  orr w1, w1, w3
  str w1, [x0, #0x4]

  ldr w1, [x0]       
  bic w1, w1, w2
  orr w1, w1, w3
  str w1, [x0]

  ret

