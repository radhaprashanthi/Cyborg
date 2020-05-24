#include<stdio.h>
#include<string.h>

/** Number representation struct */
struct nt_rep
{
    unsigned int v;
    int width;
    int range_start;
    int range_end;
    char *err_msg;
    int err;
};

/** Initialize struct variables */
void nt_rep_init(struct nt_rep *ntr)
{
    ntr->v = 0;
    ntr->width = 32;
    ntr->range_start = 0;
    ntr->range_end = 31;
    ntr->err = 0;
    ntr->err_msg = '\0';
}

/** Check if the number is valid */
int is_valid_number(struct nt_rep *ntr, long long num)
{
    if (num > 4294967295 || num < -2147483648){
        ntr->err = 1;
        ntr->err_msg = "Number cannot fit into 32 bits.";
        return 0;
    }
    return 1;
}

/** Converts decimal string to unsigned int and returns it */
unsigned int string_to_uint(struct nt_rep *ntr, char* s)
{
    int len = strlen(s);
    long long v = 0;
    if (len > 11 || len < 1) {
        ntr->err = 1;
        ntr->err_msg = "Wrong decimal input! Length should be in [1,10]";
    }
    else {
        int neg = 0, i;
        int d;
        if (s[0] == 45){
            i = 1;
            neg = 1;
        }
        else
            i = 0;
        for (i; i<len; i++) {
            v = v*10;
            d = s[i] - '0';
            v = v + d;
        }

        if (neg)
            v = v*-1;

        if (is_valid_number(ntr, v))
            return (unsigned int)v;
    }
    return 0;
}

/** Converts binary string to unsigned int and returns it */
unsigned int binary_to_uint(struct nt_rep *ntr, char* s)
{
    int v = 0;
    int len = strlen(s);
    if (len > 34 || len < 1) {
        ntr->err = 1;
        ntr->err_msg = "Wrong binary input! Length should be in [1,32]";
    }
    else {
        int i,d, base = 1;
        for (int i = len-1; i >= 0; i--) {
            d = s[i] - '0';
            d = d * base;
            v = v + d;
            base = base * 2;
        }
    }
    return v;
}

/** Converts hexadecimal string to unsigned int and returns it */
unsigned int hex_to_uint(struct nt_rep *ntr, char* s)
{
    int len = strlen(s);
    int v = 0;
    if (len > 10 || len < 3) {
        ntr->err = 1;
        ntr->err_msg = "Wrong hexadecimal input! Length should be in [1,8]";
    }
    else {
        int i;
        unsigned int d;
        unsigned int base = 1;
        for (int i = len-1; i >= 0; i--) {
            if (s[i] >= '0' && s[i] <= '9')
                d = s[i] - '0';
            else if (s[i] >= 'A' && s[i] <= 'F')
                d = s[i] - 55;
            d = d * base;
            base = base * 16;
            v = v + (unsigned int)d;
        }
    }
    return v;
}

/** Checks if bit width is valid */
int is_valid_width(struct nt_rep *ntr, unsigned width)
{
    if (width < 4 || width > 32 || width%4 != 0){
            ntr->err = 1;
            ntr->err_msg = "Bit width invalid! Please enter in 4, 8, 12, 16, 20, 24, 28, 32";
            return 0;
        }
    return 1;
}

/** Checks if range is valid */
int is_valid_range(struct nt_rep *ntr, char *range)
{
    if (range[0] > range[1] || range[0] < 0 || range[1] > 31){
        ntr->err = 1;
        ntr->err_msg = "Range invalid! Please enter range in [0,31]";
        return 0;
    }
    return 1;
}

/** Splits range from string and converts to int*/
void split_range(struct nt_rep* ntr,char *range)
{
    char *pt;
    char a[2]; int i = 0;
    pt = strtok (range,",");
    while (pt != NULL) {
        a[i] = string_to_uint(ntr, pt);
        i+=1;
        pt = strtok (NULL, ",");
    }
    if (is_valid_range(ntr, a)) {
        ntr->range_start = a[0];
        ntr->range_end = a[1];
    }
}

/** Extracts range from unsigned int */
unsigned int extract_range(struct nt_rep *ntr, unsigned int v) {
    int r0 = ntr->range_start;
    int r1 = ntr->range_end;

    unsigned int v_range = v;
    v_range = v_range >> r0;
    v_range = v_range & ((1<<(r1-r0+1))-1);

    return v_range;

}

/** Converts string number to canonical form (unsigned int) */
void string_to_canonical(struct nt_rep* ntr, char *s)
{
    int slen = strlen(s);
    unsigned int v;
    if (s[1] == 98) {
        s[1] = 48;
        s[0] = 48;
        v = binary_to_uint(ntr,s);
    }
    else if (s[1] == 120) {
        s[0] = 48;
        s[1] = 48;
        v = hex_to_uint(ntr,s);
    }
    else
        v = string_to_uint(ntr,s);

    if (!ntr->err) {
        if (ntr->range_start > 0 || ntr->range_end < 31)
            ntr->v = extract_range(ntr, v);
        else
            ntr->v = v;
    }
}

/** Check if the string is a number */
void isNumber(struct nt_rep *ntr, char *s) {
    int i = 0;
    int n = strlen(s);
    if (s[0] == 48 && (s[1] == 'b' || s[1] == 'x'))
        i = 2;
    if (s[0] == '-')
        i = 1;
    for (i; i<n; i++) {
        if (s[i] >= '0' && s[i] <= '9' && s[1] != 'x' && s[1] != 'b')
            continue;
        else if (s[1] == 'x' && (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'F'))
            continue;
        else if (s[1] == 'b' && (s[i] == '0' || s[i] == '1'))
            continue;
        else {
            ntr->err = 1;
            ntr->err_msg = "Invalid number in command line arguments";
            break;
        }
    }
}

/** Parse command line arguments and coverts number to canonical form */
void parse_command_line(struct nt_rep *ntr, char **argv, int argc)
{
    if (argc < 2) {
        ntr->err = 1;
        ntr->err_msg = " ";
    }
    else if (argc > 6) {
        ntr->err = 1;
        ntr->err_msg = "Too many command line arguments";
    }
    else {
        int num_count = 0;
        char *num;
        for (int i = 1; i < argc; i++){
            if (strcmp("-b", argv[i]) == 0){
                if (i+1 < argc) {
                    unsigned int width = string_to_uint(ntr, argv[i+1]);
                    if (is_valid_width(ntr, width))
                        ntr->width = width;
                    i++;
                }
                else {
                    ntr->err = 1;
                    ntr->err_msg = "No input for -b";
                    break;
                }
            }
            else if (strcmp("-r", argv[i]) == 0) {
                if (i+1 < argc) {
                    split_range(ntr, argv[i+1]);
                    i++;
                }
                else {
                    ntr->err = 1;
                    ntr->err_msg = "No input for -r";
                    break;
                }
            }
            else if (num_count == 0) {
                num_count ++;
                isNumber(ntr, argv[i]);
                if (!ntr->err) {
                    num = argv[i];
                }
            }
            else {
                ntr->err = 1;
                ntr->err_msg = "Invalid command line arguments";
                break;
            }
        }
        if (!ntr->err)
            string_to_canonical(ntr, num);
    }
}

/** Converts unsigned int to 32-bit binary format */
void uint_to_bin32(unsigned int num, char *bin)
{
    for (int j = 0; j < 32; j++){
        bin[j] = 48;
    }
    int i = 31;

    while(num != 0) {
        bin[i] = num%2 + 48;
        i--;
        num = num/2;
    }
}

/**
Prints binary format of unsigned int in 2 formats by bit width.
1. Human readable binary format
2. C readable binary format
*/
void print_bin(struct nt_rep *ntr)
{
    char ret_bin[33];
    ret_bin[32] = '\0';
    uint_to_bin32(ntr->v, ret_bin);

    int start = 32-ntr->width;
    int n1 = ntr->width+(ntr->width/4)-1;
    int n2 = ntr->width + 2;
    char hr[n1+1]; hr[n1] = '\0';
    char chr[n2+1]; chr[n2] = '\0';
    for (int l = 0; l < n1; l++)
        hr[l] = 48;
    for (int l = 0; l < n2; l++)
        chr[l] = 48;

    chr[1] = 98;
    int k = 0;
    int m = 2;
    for (int j = start; j < 32; j++){
        if (j%4 == 0 && j != start){
            hr[k] = 32;
            hr[k+1] = ret_bin[j];
            k = k + 2;
        }
        else {
            hr[k] = ret_bin[j];
            k++;
        }
        chr[m] = ret_bin[j];
        m++;
    }

    printf("%s (base 2)\n",hr);
    printf("%s (base 2)\n",chr);
}

/** Converts unsigned int to hexadecimal format */
void uint_to_hex(char *hex, unsigned int n) {
    int i = 7, temp  = 0;

    while(n!=0) {
        temp = n % 16;
        if(temp < 10)
            hex[i] = temp + 48;
        else
            hex[i] = temp + 55;
        i--;
        n = n/16;
    }
}

/** Prints hexadecimal format of unsigned int by width as string */
void print_hex(struct nt_rep *ntr)
{
    char hex[9];
    hex[8] = '\0';
    for (int j = 0; j < 8; j++)
        hex[j] = '0';

    uint_to_hex(hex, ntr->v);

    int start = 8-(ntr->width/4);
    int len = 2 + ntr->width/4;

    char chex[len+1];
    chex[len] = '\0';
    for (int m = 0; m < len; m++)
        chex[m] = 48;

    chex[1] = 120;
    int k = 2;
    for (int j = start; j < 8; j++) {
        chex[k] = hex[j];
        k++;
    }
    printf("%s (base 16)\n",chex);
}

/**
Converts unsigned int to string and return the index where the string ends.
This index is used to truncate the beginning zeroes of unsigned int string.
*/
int uint_to_string(char *uint, unsigned int v)
{
    for (int j = 0; j < 10; j++)
        uint[j] = '0';
    int k = 9;
    if (v == 0)
        k = 8;
    else {
        while (v > 0) {
            uint[k] = v % 10 + 48;
            k--;
            v = v/10;
        }
    }
    return 9-k;
}

/** Function for 2's complement of binary string */
void twos_complement(char *dec){
    char r;
    int flag = 0,i,s;
    for(i = strlen(dec)-1; i >= 0; i--)
    {
        s = dec[i]-48;
        if(s == 1 && flag == 0)
        {
            flag = 1;
        }
        else if(flag == 1)
        {
            r = !s+48;
            dec[i] = r;
        }
    }
}

/** Prints unsinged int as string */
void print_uint(struct nt_rep* ntr)
{
    char uint[11];
    uint[10] = '\0';
    int uint_len = 0;
    if (ntr->width < 32) {
        int shift = 32-ntr->width;
        unsigned int v1 = ntr->v;
        v1 = v1 << shift;
        v1 = v1 >> shift;
        uint_len = uint_to_string(uint, v1);
    }
    else
        uint_len = uint_to_string(uint, ntr->v);

    char uint_print[uint_len+1];
    uint_print[uint_len] = '\0';

    for (int j = 0; j < uint_len; j++)
        uint_print[j] = '0';

    int j = 0;
    for (int i= 10-uint_len; i<10; i++){
        uint_print[j] = uint[i];
        j++;
    }
    printf("%s (base 10 unsigned)\n",uint_print);
}

/** Prints signed int in string format
1. Converts unsigned int to binary
2. If MSB is negative, take 2's complement and convert to signed int
3. Converts signed int to string adding - sign
*/
void print_sint(struct nt_rep* ntr)
{
    unsigned int v_wid;
    int neg = 0;
    char ret_bin[33];
    ret_bin[32] = '\0';
    uint_to_bin32(ntr->v, ret_bin);

    if (ntr->width < 32) {
        int start = 32-ntr->width;
        char bn_wid[ntr->width+1];
        bn_wid[ntr->width] = '\0';

        int j = 0;
        for (int i = start; i < 32; i++) {
            bn_wid[j] = ret_bin[i];
            j++;
        }
        if (bn_wid[0] == '1')
            neg = 1;
        if (neg)
            twos_complement(bn_wid);
        v_wid = binary_to_uint(ntr, bn_wid);
    }
    else {
        if (ret_bin[0] == '1')
            neg = 1;
        if (neg)
            twos_complement(ret_bin);
        v_wid = binary_to_uint(ntr, ret_bin);
    }

    //Print signed int
    char sint[11];
    sint[10] = '\0';
    int k = uint_to_string(sint, v_wid);
    if (neg) {
        char sint_neg[k+2];
        sint_neg[k+1] = '\0';
        sint_neg[0] = '-';
        for (int i = 1; i <= k; i++)
            sint_neg[i] = '0';

        int j = 1;
        for (int i = 10-k; i<10; i++){
            sint_neg[j] = sint[i];
            j++;
        }
        printf("%s (base 10 signed)\n",sint_neg);
    }
    else {
        char sint_pos[k+1];
        sint_pos[k] = '\0';
        for (int j = 0; j < k; j++)
            sint_pos[j] = '0';
        int j = 0;
        for (int i = 10-k; i<10; i++){
            sint_pos[j] = sint[i];
            j++;
        }
        printf("%s (base 10 signed)\n",sint_pos);
    }
}

/** Prints different formats of unsigned int as strings */
void print_nt(struct nt_rep *ntr)
{
    if (!ntr->err) {
        print_bin(ntr);
        print_hex(ntr);
        print_uint(ntr);
        print_sint(ntr);
    }
    else {
        if (ntr->err_msg != " ")
            printf("Error: %s\n", ntr->err_msg);

        printf("\nUSAGE: ./nt -b <bit width> -r <range of bits> <value>\n");
        printf("bit width (optional): 4, 8, 12, 16, 20, 24, 28, 32 (default)\n");
        printf("range of bits (optional): the first value in the range specifies the least significant bit (the rightmost bit) and the second value in the range specifies the most significant bit (the leftmost bit). The range values start at 0 and they are inclusive.\n");
        printf("value: prepend 0b if binary, 0x if hex, otherwise parsed as an integer with respect to the bid width\n");
    }
}

/**Main function to call all the required functions */
int main(int argc, char **argv)
{
    struct nt_rep ntr;
    nt_rep_init(&ntr);
    parse_command_line(&ntr, argv, argc);
    print_nt(&ntr);
}
