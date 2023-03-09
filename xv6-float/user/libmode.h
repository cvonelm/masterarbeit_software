
struct __attribute__((packed)) mode{
   unsigned char template[128];
   unsigned long long sp;
   void *(*pcallhandler)(void *);
   int (*syscallhandler)(int a0, int a1, int a2, int a3, int a4, int a5);
   unsigned long long stack[16];
 };
 
 extern void *childcall(struct mode* entry, int modeid, void*(fp)(void*), void *b);
 extern int scall(int a0, int a1, int a2, int a3, int a4, int a5);
 
 extern void * parentcall(struct mode* entry, int target, void *arg);
 
 unsigned char user_pcalltemplate[] = {
   0x97, 0x02, 0x00, 0x00, 0x93, 0x82, 0x02, 0x08, 0x03, 0xb3, 0x02, 0x00,
   0x41, 0x13, 0x23, 0xb0, 0x62, 0x00, 0x73, 0x2e, 0x10, 0x80, 0x23, 0x30,
   0xc3, 0x01, 0xf3, 0x2e, 0x00, 0x80, 0x2b, 0xce, 0x0e, 0x00, 0x23, 0x34,
   0xc3, 0x01, 0x41, 0x11, 0x06, 0xe4, 0x22, 0xe0, 0x73, 0x2e, 0x20, 0x80,
   0x13, 0x5e, 0xfe, 0x03, 0x63, 0x07, 0x0e, 0x00, 0x03, 0xbe, 0x82, 0x00,
   0xe7, 0x00, 0x0e, 0x00, 0x31, 0xa0, 0x03, 0xbe, 0x02, 0x01, 0xe7, 0x00,
   0x0e, 0x00, 0x09, 0xa0, 0x97, 0x02, 0x00, 0x00, 0x93, 0x82, 0x42, 0x03,
   0x03, 0xb3, 0x02, 0x00, 0x03, 0x3e, 0x03, 0x00, 0x73, 0x10, 0x1e, 0x80,
   0xf3, 0x2e, 0x00, 0x80, 0x03, 0x3e, 0x83, 0x00, 0x2b, 0xde, 0x0e, 0x00,
   0x41, 0x03, 0x23, 0xb0, 0x62, 0x00, 0xa2, 0x60, 0x02, 0x64, 0x41, 0x01,
   0x0b, 0x10, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00
 };

void *default_pcall_handler(void *arg)
 {
   return NULL;
 }
 
 struct mode *modeinit()
 {
   struct mode *mode = malloc(sizeof(struct mode));
   memcpy(mode->template, user_pcalltemplate, 128);
   mode->sp = (unsigned long long)&mode->stack[15];
   mode->syscallhandler = &scall;
   mode->pcallhandler = &default_pcall_handler;
   mentry(mode->template);
   return mode;
 }
 int modecreate(struct mode *mode)
 {
   int child = mcreate();
   permchange(child, mode, sizeof(struct mode), 0);
   return child;
 }
 int modedelete(int child)
 {
   return mdelete(child);
 }
 void modefinish(struct mode *mode)
 {
   free(mode);
 }
static inline uint64_t r_ccalladdr() 
   { 
     uint64_t x; 
     asm volatile("csrr %0, ccalladdr" : "=r" (x) ); 
     return x; 
   }
   
   static inline void
   w_ccalladdr(uint64_t x)
   {
     asm volatile("csrw ccalladdr, %0" : : "r" (x));
   }
 //Mode ID Register
 static inline uint64_t
 r_targetmode()
 {
   uint64_t x;
   asm volatile("csrr %0, targetmode" : "=r" (x) );
   return x;
 }

 static inline void
 w_targetmode(uint64_t x)
 {
   asm volatile("csrw targetmode, %0" : : "r" (x));
 }
 // Supervisor Trap Cause
 static inline uint64_t
 r_scause()
 {
   uint64_t x;
   asm volatile("csrr %0, scause" : "=r" (x) );
   return x;
 }

