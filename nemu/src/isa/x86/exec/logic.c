#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  //TODO();
  rtl_and(&s0,&id_dest->val,&id_src->val);
  /*if(s0==0){
    reg_f(ZF)=1;
  }
  else reg_f(ZF)=0;*/
  rtl_update_ZFSF(&s0,id_dest->width);
  reg_f(CF)=0;
  reg_f(OF)=0;
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  reg_f(CF)=0;
  reg_f(OF)=0;
  rtl_and(&id_dest->val,&id_dest->val,&id_src->val);
  //assert(id_dest->type==OP_TYPE_REG);
  //rtl_sr(id_dest->reg,&id_dest->val,id_dest->width);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  operand_write(id_dest,&id_dest->val);
  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&id_dest->val,&id_dest->val,&id_src->val);
  //if(id_dest->type==OP_TYPE_REG){
	//rtl_sr(id_dest->reg,&id_dest->val,id_dest->width);
  operand_write(id_dest,&id_dest->val);
  reg_f(CF)=0;
  reg_f(OF)=0;
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&s0,&id_dest->val,&id_src->val);
  id_dest->val=s0;
  reg_f(CF)=0;
  reg_f(OF)=0;
  rtl_update_ZFSF(&s0,id_dest->width);
  operand_write(id_dest,&id_dest->val);
  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  switch(id_dest->width){
    case 4:
      rtl_sar(&s0,&id_dest->val,&id_src->val);
      break;
    case 2:{
      int16_t temp1=id_dest->val;
      temp1=temp1>>id_src->val;
      s0=temp1;
      break;
      }
    case 1:{
      int8_t temp2=id_dest->val;
      temp2=temp2>>id_src->val;
      s0=temp2;
      break;
      }
    default:assert(0);
  }
  operand_write(id_dest,&s0);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  
  rtl_shl(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  print_asm_template2(shl);
}
make_EHelper(rol){
  switch(id_dest->width){
    case 1:{
      int8_t temp=id_dest->val;
      for(int i=0;i<id_src->val;i++){
        s0=temp>>7;
        temp=(temp<<1)+s0;
        reg_f(CF)=s0;
      }
      s1=temp;
      break;
    }
    case 2:{
      int16_t temp=id_dest->val;
      for(int i=0;i<id_src->val;i++){
        s0=temp>>15;
        temp=(temp<<1)+s0;
        reg_f(CF)=s0;
      }
      s1=temp;
      break;
    }
    case 4:{
      int32_t temp=id_dest->val;
      for(int i=0;i<id_src->val;i++){
        s0=temp>>31;
        temp=(temp<<1)+s0;
        reg_f(CF)=s0;
      }
      s1=temp;
      break;
    }
    default: assert(0);
  }
  operand_write(id_dest,&s1);
}
make_EHelper(ror){
  switch(id_dest->width){
    case 1:{
      int8_t temp=id_dest->val;
      int8_t x;
      for(int i=0;i<id_src->val;i++){
        x=temp&0x01;
        temp=temp>>1;
        temp=  x==1?  temp&0xff : temp&0x7f;
        reg_f(CF)=x;
      }
      s1=temp;
      break;
    }
    case 2:{
      int16_t temp=id_dest->val;
      int8_t x;
      for(int i=0;i<id_src->val;i++){
        x=temp&0x01;
        temp=temp>>1;
        temp= x==1? temp&0xffff : temp&0x7fff;
        reg_f(CF)=x;
      }
      s1=temp;
      break;
    }
    case 4:{
      int32_t temp=id_dest->val;
      int8_t x;
      for(int i=0;i<id_src->val;i++){
        x=temp&0x01;
        temp=temp>>1;
        temp= x==1? temp&0xffffffff : temp&0x7fffffff;
        reg_f(CF)=x;
      }
      s1=temp;
      break;
    }
    default:assert(0);
  }
  operand_write(id_dest,&s1);
  print_asm_template2(ror);
}
make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&s0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&s0);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  //printf("dest %x \n",id_dest->val);
  id_dest->val=~id_dest->val;
  //printf("~dest %x \n",id_dest->val);
  
  operand_write(id_dest,&id_dest->val);
  print_asm_template1(not);
}
