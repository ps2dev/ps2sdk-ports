#ifdef __cplusplus
extern "C" {
#endif

void rmallocInit();
void *rmalloc(int size);
unsigned int rallocated();
void rfree(void *user);

#ifdef __cplusplus
}
#endif
