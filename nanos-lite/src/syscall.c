#include "common.h"
#include "syscall.h"
void sys_yield(_Context *c){
  _yield();
  c->GPRx=0;
}
void sys_exit(_Context *c){
  int temp=c->GPR2;
  printf("gp2: %d\n",c->GPR2);
  _halt(temp);
}
_Context* __am_irq_handle(_Context *c);
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case SYS_exit: sys_exit(c);break;
    case SYS_yield: sys_yield(c);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  _Context *next=__am_irq_handle(c);
  return next;
}
