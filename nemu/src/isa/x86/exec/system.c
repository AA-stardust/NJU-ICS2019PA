#include "cpu/exec.h"

make_EHelper(lidt) {
  //TODO();
  if(decinfo.isa.is_operand_size_16){
    cpu.idtr.limit=vaddr_read(id_dest->addr,2);
    cpu.idtr.base=vaddr_read(id_dest->addr+2,3);
  }
  else{
    cpu.idtr.limit=vaddr_read(id_dest->addr,2);
    cpu.idtr.base=vaddr_read(id_dest->addr+2,4);
  }
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  //TODO();
  switch(id_dest->reg){
    case 0:
      cpu.CR0=id_src->val;
      break;
    case 3:
      cpu.CR3=id_src->val;
      break;
    default:
      Assert(0,"Should not reach here (cr%d)",id_dest->reg);
  }
  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  //TODO();
  switch(id_src->reg){
    case 0:
      operand_write(id_dest,&cpu.CR0);
      break;
    case 3:
      operand_write(id_dest,&cpu.CR3);
      break;
    default:
      Assert(0,"Should not reach here (cr%d)",id_dest->reg);
      
  }
  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}
void raise_intr(uint32_t NO, vaddr_t ret_addr);
make_EHelper(int) {
  //TODO();
  //printf("decinfo.seq_pc: %x\n",decinfo.seq_pc);
  //printf("cpu.pc: %x\n",cpu.pc);
  raise_intr(id_dest->val,decinfo.seq_pc);
  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&decinfo.jmp_pc);
  //printf("%x\n",decinfo.jmp_pc);
  rtl_pop(&cpu.cs);
  rtl_j(decinfo.jmp_pc);
  rtl_pop(&cpu.EFLAGS.value);
  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  switch(id_dest->width){
    case 1:
      t0=pio_read_b(id_src->val);
      break;
    case 2:
      t0=pio_read_w(id_src->val);
      break;
    case 4:
      t0=pio_read_l(id_src->val);
      break;
    default:
      assert(0);
  }
  operand_write(id_dest,&t0);
  //TODO();

  print_asm_template2(in);
}

make_EHelper(out) {
  //TODO();
  rtl_li(&t1,id_dest->val);
  switch(id_dest->width){
    case 1:
      pio_write_b(t1,id_src->val);
      break;
    case 2:
      pio_write_w(t1,id_src->val);
      break;
    case 4:
      pio_write_l(t1,id_src->val);
      break;
    default:
      assert(0);
  }
  print_asm_template2(out);
}
