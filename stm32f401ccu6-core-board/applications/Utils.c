#ifdef __cplusplus
extern "C" {
#endif

void arraycopy(
        const char src[], const int srcPos
        , char dest[], const int destPos
        , const int length) {

    for(int i = 0; i < length; i++) {
        dest[i + destPos] = src[i + srcPos];
    }
}


/**
 * split src by \r\n
 * des array mapped every \n.
 * return count of \r\n
 */
int split(const char* src, const int lenSrc, int* des) {
    if (lenSrc <= 2) {
        return 0;
    }

    if (src[0] == '\r' && src[1] == '\n') {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < lenSrc-1; i++) {
        if (src[i] == '\r' && src[i+1] == '\n') {
            des[count++] = i+1;
        }
    }

    return count;
}

#ifdef __cplusplus
}
#endif
