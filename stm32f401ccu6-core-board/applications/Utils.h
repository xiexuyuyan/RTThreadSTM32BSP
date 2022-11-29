#ifdef __cplusplus
extern "C" {
#endif

void arraycopy(
        const char src[], int srcPos
        , char dest[], int destPos
        , int length);

int split(const char* src, int lenSrc, int* des);

#ifdef __cplusplus
}
#endif
