#include "cpu/exec.h"

make_EHelper(add) {
  //TODO();
  //s0 is result, s1 is eflags
  rtl_add(&s0,&id_dest->val,&id_src->val);

  rtl_is_add_carry(&s1,&s0,&id_dest->val);
  rtl_set_CF(&s1);

  rtl_is_add_overflow(&s1,&s0,&id_dest->val,&id_src->val,id_dest->width);
  reg_f(OF)=s1;

  rtl_update_ZFSF(&s0,id_dest->width);
  
  //printf("SF: %-2d ZF: %-2d OF: %-2d CF: %-2d\n",reg_f(SF),reg_f(ZF),reg_f(OF),reg_f(CF));
  
  id_dest->val=s0;
  operand_write(id_dest,&id_dest->val);
  
  print_asm_template2(add);
}

make_EHelper(sub) {
  //TODO();
  rtl_sub(&s0,&id_dest->val,&id_src->val);

  rtl_is_sub_overflow(&s1,&s0,&id_dest->val,&id_src->val,id_dest->width);
  rtl_set_OF(&s1);

  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  
  rtl_is_sub_carry(&s1,&s0,&id_dest->val);
  rtl_set_CF(&s1);

  //printf("SF: %-2d ZF: %-2d OF: %-2d CF: %-2d\n",reg_f(SF),reg_f(ZF),reg_f(OF),reg_f(CF));
  id_dest->val=s0;
  operand_write(id_dest,&id_dest->val);
  print_asm_template2(sub);
}

make_EHelper(cmp) {
  //TODO();
  //printf("val:%d  src:%d\n",id_dest->val,id_src->val);
  rtl_sub(&s0,&id_dest->val,&id_src->val);
  
  rtl_is_sub_carry(&s1,&s0,&id_dest->val);
  rtl_set_CF(&s1);
  
  rtl_update_ZFSF(&s0,id_dest->width);
  rtl_is_sub_overflow(&s1,&s0,&id_dest->val,&id_src->val,id_dest->width);
  reg_f(OF)=s1;
  //rtl_is_sub_carry(&s0,id_dest->val-id_src->val,id_dest->val);
  //reg_f(CF)=s0;
  //printf("SF: %-2d ZF: %-2d OF: %-2d CF: %-2d\n",reg_f(SF),reg_f(ZF),reg_f(OF),reg_f(CF));
  print_asm_template2(cmp);
}

make_EHelper(inc) {
  //TODO();
  s1=1;
  rtl_add(&s0,&id_dest->val,&s1);
  rtl_is_add_overflow(&s1,&s0,&id_dest->val,&id_src->val,id_dest->width);
  rtl_set_OF(&s1);
  rtl_is_sub_carry(&s1,&s0,&id_dest->val);
  rtl_set_CF(&s1);
  rtl_update_ZFSF(&s0,id_dest->width);
  //rtl_sr(id_dest->reg,&id_dest->val,id_dest->width);
  operand_write(id_dest,&s0);
  print_asm_template1(inc);
}

make_EHelper(dec) {
  //TODO();
  s1=1;
  rtl_sub(&s0,&id_dest->val,&s1);
  rtl_is_sub_overflow(&s1,&s0,&id_dest->val,&id_src->val,id_dest->width);
  rtl_set_OF(&s1);
  rtl_is_sub_carry(&s1,&s0,&id_dest->val);
  rtl_set_CF(&s1);
  operand_write(id_dest,&s0);
  rtl_update_ZFSF(&s0,id_dest->val);
  print_asm_template1(dec);
}

make_EHelper(neg) {
  //TODO();
  if(id_dest->val==0){
    t0=0;
    rtl_set_CF(&t0);
  }
  else{
      t0=1;
      rtl_set_CF(&t0);
    }
  t1=-id_dest->val;
  operand_write(id_dest,&t1);
  rtl_update_ZFSF(&t1,id_dest->width);
  if((id_dest->val==-128&&id_dest->width==1)||(id_dest->val=-32768&&id_dest->width==2)||(id_dest->val==-2147483648&&id_dest->width==4)){
    t0=1;
    rtl_set_OF(&t0);
  }
  else{
    t0=0;
    rtl_set_OF(&t0);
  }
  print_asm_template1(neg);
}


make_EHelper(adc) {
  // s0 = dest + src
  rtl_add(&s0, &id_dest->val, &id_src->val);
  // s1 = s0 + CF
  rtl_get_CF(&s1);
  rtl_add(&s1, &s0, &s1);

  operand_write(id_dest, &s1);

  if (id_dest->width != 4) {
    rtl_andi(&s1, &s1, 0xffffffffu >> ((4 - id_dest->width) * 8));
  }

  rtl_update_ZFSF(&s1, id_dest->width);

  // update CF
  rtl_is_add_carry(&s1, &s1, &s0);
  rtl_is_add_carry(&s0, &s0, &id_dest->val);
  rtl_or(&s0, &s0, &s1);
  rtl_set_CF(&s0);

  // update OF
  rtl_is_add_overflow(&s0, &s1, &id_dest->val, &id_src->val, id_dest->width);
  rtl_set_OF(&s0);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  // s0 = dest - src
  rtl_sub(&s0, &id_dest->val, &id_src->val);
  // s1 = s0 - CF
  rtl_get_CF(&s1);
  rtl_sub(&s1, &s0, &s1);

  operand_write(id_dest, &s1);

  if (id_dest->width != 4) {
    rtl_andi(&s1, &s1, 0xffffffffu >> ((4 - id_dest->width) * 8));
  }

  rtl_update_ZFSF(&s1, id_dest->width);

  // update CF
  rtl_is_sub_carry(&s1, &s1, &s0);
  rtl_is_sub_carry(&s0, &s0, &id_dest->val);
  rtl_or(&s0, &s0, &s1);
  rtl_set_CF(&s0);

  // update OF
  rtl_is_sub_overflow(&s0, &s1, &id_dest->val, &id_src->val, id_dest->width);
  rtl_set_OF(&s0);

  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&s0, R_EAX, id_dest->width);
  rtl_mul_lo(&s1, &id_dest->val, &s0);

  switch (id_dest->width) {
    case 1:
      rtl_sr(R_AX, &s1, 2);
      break;
    case 2:
      rtl_sr(R_AX, &s1, 2);
      rtl_shri(&s1, &s1, 16);
      rtl_sr(R_DX, &s1, 2);
      break;
    case 4:
      rtl_mul_hi(&s0, &id_dest->val, &s0);
      rtl_sr(R_EDX, &s0, 4);
      rtl_sr(R_EAX, &s1, 4);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&s0, R_EAX, id_dest->width);
  rtl_imul_lo(&s1, &id_dest->val, &s0);

  switch (id_dest->width) {
    case 1:
      rtl_sr(R_AX, &s1, 2);
      break;
    case 2:
      rtl_sr(R_AX, &s1, 2);
      rtl_shri(&s1, &s1, 16);
      rtl_sr(R_DX, &s1, 2);
      break;
    case 4:
      rtl_imul_hi(&s0, &id_dest->val, &s0);
      rtl_sr(R_EDX, &s0, 4);
      rtl_sr(R_EAX, &s1, 4);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(&s0, &id_src->val, id_src->width);
  rtl_sext(&s1, &id_dest->val, id_dest->width);

  rtl_imul_lo(&s0, &s1, &s0);
  operand_write(id_dest, &s0);

  print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3) {
  rtl_sext(&s0, &id_src->val, id_src->width);
  rtl_sext(&s1, &id_src2->val, id_src->width);

  rtl_imul_lo(&s0, &s1, &s0);
  operand_write(id_dest, &s0);

  print_asm_template3(imul);
}

make_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(&s0, R_AX, 2);
      rtl_div_q(&s1, &s0, &id_dest->val);
      rtl_sr(R_AL, &s1, 1);
      rtl_div_r(&s1, &s0, &id_dest->val);
      rtl_sr(R_AH, &s1, 1);
      break;
    case 2:
      rtl_lr(&s0, R_AX, 2);
      rtl_lr(&s1, R_DX, 2);
      rtl_shli(&s1, &s1, 16);
      rtl_or(&s0, &s0, &s1);
      rtl_div_q(&s1, &s0, &id_dest->val);
      rtl_sr(R_AX, &s1, 2);
      rtl_div_r(&s1, &s0, &id_dest->val);
      rtl_sr(R_DX, &s1, 2);
      break;
    case 4:
      rtl_lr(&s0, R_EAX, 4);
      rtl_lr(&s1, R_EDX, 4);
      rtl_div64_q(&cpu.eax, &s1, &s0, &id_dest->val);
      rtl_div64_r(&cpu.edx, &s1, &s0, &id_dest->val);
      break;
    default: assert(0);
  }

  print_asm_template1(div);
}

make_EHelper(idiv) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(&s0, R_AX, 2);
      rtl_idiv_q(&s1, &s0, &id_dest->val);
      rtl_sr(R_AL, &s1, 1);
      rtl_idiv_r(&s1, &s0, &id_dest->val);
      rtl_sr(R_AH, &s1, 1);
      break;
    case 2:
      rtl_lr(&s0, R_AX, 2);
      rtl_lr(&s1, R_DX, 2);
      rtl_shli(&s1, &s1, 16);
      rtl_or(&s0, &s0, &s1);
      rtl_idiv_q(&s1, &s0, &id_dest->val);
      rtl_sr(R_AX, &s1, 2);
      rtl_idiv_r(&s1, &s0, &id_dest->val);
      rtl_sr(R_DX, &s1, 2);
      break;
    case 4:
      rtl_lr(&s0, R_EAX, 4);
      rtl_lr(&s1, R_EDX, 4);
      rtl_idiv64_q(&cpu.eax, &s1, &s0, &id_dest->val);
      rtl_idiv64_r(&cpu.edx, &s1, &s0, &id_dest->val);
      break;
    default: assert(0);
  }

  print_asm_template1(idiv);
}
