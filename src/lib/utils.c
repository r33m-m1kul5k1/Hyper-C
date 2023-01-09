void strcpy(char *dst, const char *src)
{
    for (;*src != '\0'; src++, dst++)
    {
        *dst = *src;
    }
    *dst = '\0';
}

int strlen(const char *str)
{
    int length = 0;
    while(str[length])
        length++;

    return length;
}

const char *get_number_start(const char *str)
{
    if (strlen(str) == 1)
    {
        return str;
    }

    while (*str == '0')
    {
        str++;
    }

    return str;
}