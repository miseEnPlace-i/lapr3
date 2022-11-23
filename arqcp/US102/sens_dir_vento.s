.section .text
  .global sens_dir_vento # char sens_dir_vento(unsigned short ult_dir_vento, short comp_rand)

# rdi ult_dir_vento
# rsi comp_rand

sens_dir_vento:

  movb %sil, %al # al = random component
  cbtw # cast byte to word

  testw %ax, %ax
  je compRandZero

  testw %ax, %ax
  js compRandNeg

continue:

  movb VELC_SENSOR_DIR_WIND_MAX_VARIATION (%rip), %cl # cl = max variation
  incb %cl # cl = max variation + 1

  divb %cl # divide random component by max variation (remainder in %ah)

  shrw $8, %ax # get the value to right position (%al)

  addb %dil, %al # add to last random value

  jmp end

compRandZero:

  movb %dil, %al # al = last random value
  jmp end

compRandNeg:
  negw %ax # make positive
  jmp continue

end:
ret
