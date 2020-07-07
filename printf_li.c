/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_li.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebresser <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/09 11:00:03 by ebresser          #+#    #+#             */
/*   Updated: 2020/06/09 15:56:56 by ebresser         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>

/*
 * ***************************************
 * to insert in libftprintf.h
 * ****************************************
 */
struct fields{
    int     flagzero;
    int     flagminus;
    int     width;
    int     point;
    int     precision;
    char    specifier;
};
static void     ft_putchar(char c, int *p);
static void     ft_putstr(char *s, int *p, int qtt);
static void		cut(unsigned int number, int *p);
static void		cuthex(size_t number, int capitalized, int *p);
static void     ft_putnbr(int n, int *p);
static void     ft_putnbr_u(unsigned int n, int *p);
static void     ft_putnbr_hex(size_t h, int capitalized, int *p);
static int      len_int(int a); 
static int      len_u(unsigned int u);
static int      len_hex(unsigned int h); 
static int      len_add(size_t add);
static int      strlen(char *s); 
static char*    strcpy(char *s, int qtt);
static void     ft_printspacezero(int type, int qtt, int *p); 
static void     ft_printint(va_list *p_ap, int signal, int *p, struct fields *f);
static void     ft_printhex(va_list *p_ap, int capitalized, int *p, struct fields *f);
static void     ft_printpointer(va_list *p_ap, int *p, struct fields *f);
static void     ft_printstr(va_list *p_ap, int *p, struct fields *f);
static void     ft_printchar(va_list *p_ap, int *p, struct fields *f);
static void     ft_strformat_init(struct fields *f);
static int      ft_fieldstorage(va_list *p_ap, const char *fmt,int *fmt_inc, struct fields *f);
static int      ft_fillflags(const char *fmt, int *fmt_inc, struct fields *f);
static void     ft_fillwidth(va_list *p_ap, const char *fmt,int *fmt_inc,struct fields *f);
static int      ft_fillprecision(va_list *p_ap, const char *fmt, int *fmt_inc, struct fields *f);
static int      ft_fillspecifier(const char *fmt, int *fmt_inc, struct fields *f);
static void     ft_specifier_redirect(va_list *p_ap, char sp, int *p, struct fields *f);
int             ft_printf(const char *fmt, ...);

/*
 * ***************************************
 * aux functions
 * ****************************************
 */

static void     ft_putchar(char c, int *p)
{
    write(1, &c, 1);
    (*p)++;
}
static void     ft_putstr(char *s, int *p, int qtt)
{
    if (s)
        while (*s && qtt)
        {
            ft_putchar(*s++, p);
            qtt--;
        }
}
static void		cut(unsigned int number, int *p)
{
	int		div;
	int		mod;
	char	c;

	div = number / 10;
	mod = number % 10;
	if (div != 0)
		cut(div, p);
	c = '0' + mod;
	ft_putchar(c, p);
}
static void		cuthex(size_t number, int capitalized, int *p)
{
	size_t		div;
	int		mod;
	char	c;

	div = number / 16;
	mod = number % 16;
	if (div != 0)
		cuthex(div, capitalized, p);
    if(mod < 10)
        c = '0' + mod;
    else
    {
        if(capitalized)
            c = 'A' + (mod - 10);
        else
            c = 'a' + (mod - 10);
    }
	ft_putchar(c, p);
}
static void     ft_putnbr_hex(size_t h, int capitalized, int *p)
{
    if (h != 0)
            cuthex(h,capitalized, p);
	else
		ft_putchar('0', p);
}

static void     ft_putnbr(int n, int *p)
{
	unsigned int u;

    if (n != 0)
	{
		if (n < 0)
		{
            u = -n;
            cut(u, p);
		}
		else
        {
            u = n; 
            cut(u, p);
        }

	}
	else
		ft_putchar('0', p);
}
static void     ft_putnbr_u(unsigned int u, int *p)
{

    if (u != 0)
        cut(u, p);
	else
		ft_putchar('0', p);
}
static int      len_hex(unsigned int h) 
{
    int length;
    
    length = 1;
    while (h > 15)
    {
        h = h/16;
        length++;
    }
    return (length);
}
static int      len_add(size_t add) 
{
    int length;
    
    length = 1;
    while (add > 15)
    {
        add = add/16;
        length++;
    }
    return (length);
}
static int      len_int(int a) 
{
    int length;
    
    length = 1;
    if (a < 0)
        a = -a;
    while (a > 9)
    {
        a = a/10;
        length++;
    }
    return (length);
}
static int      len_u(unsigned int u) 
{
    int length;
    
    length = 1;
    while (u > 9)
    {
        u = u/10;
        length++;
    }
    return (length);
}
static void     ft_printspacezero(int type, int qtt, int *p) 
{
    char c;

    if(type == 0)
        c = '0';
    else
        c = ' ';
    while(qtt > 0)
    {
        ft_putchar(c, p);
        qtt--;
    }
}
/* ADDRESS POINTER - p */
static void     ft_printpointer(va_list *p_ap, int *p, struct fields *f) 
{
    //Nao faz sentido precisao nem flag zero!
    //fazer fillspecifier rejeitar. Ela já é uma static int: Usar ela!
    int add_len;
    size_t add;
    int space;

    add = (size_t)va_arg(*p_ap, void *);//cplusplus.com/cstdio/printf/
    add_len = len_add(add) + 2;//'0x'
    space = f->width - add_len;
    if(f->flagminus)
    {
        ft_putstr("0x",p, 2);
        ft_putnbr_hex(add, 0, p);
        ft_printspacezero(1, space, p);
    }
    else
    {
        ft_printspacezero(1, space, p);
        ft_putstr("0x",p, 2);
        ft_putnbr_hex(add, 0, p);
    }
}

/* HEX - h, H*/
static void     ft_printhex(va_list *p_ap, int capitalized, int *p, struct fields *f)
{
    unsigned int h;
    int h_len;
    int space;
    int zero;

    h = va_arg(*p_ap, unsigned int);
    h_len = len_hex(h);
    if (f->point) //se tiver precisao, quem conta eh ela (p efeito de 0)
    {
        zero = f->precision - h_len;
        if (zero > 0)
            space = f->width - f->precision;
        else
            space = f->width - h_len;
        if(!(f->flagminus))//alinhado a direita
        {
            ft_printspacezero( 1, space, p);
            if(zero > 0)
                ft_printspacezero( 0, zero, p);
            ft_putnbr_hex(h, capitalized, p);
        }
        else
        {
            if(zero > 0)
                ft_printspacezero( 0, zero, p);
            ft_putnbr_hex(h, capitalized, p);
            ft_printspacezero( 1, space, p);
        }
    }
    else
    {
        space = f->width - h_len;
        zero = space;
        if(f->flagzero)
        {
            ft_printspacezero(0, zero, p);
            ft_putnbr_hex(h, capitalized, p);
        }
        else
        {
            if(f->flagminus)
            {
                ft_putnbr_hex(h,capitalized, p);
                ft_printspacezero(1, space, p);
            }
            else
            {
                ft_printspacezero(1, space, p);
                ft_putnbr_hex(h,capitalized, p);
            }
        }
    }
}

/* INT - d, i, u*/ 
static void     ft_printint(va_list *p_ap, int signal, int *p, struct fields *f)
{ 
        //signed and unsigned, depend on signal
        int d;
        int d_len;
        int neg;
        int space;
        int zero;
        
        neg = 0;
        if(!signal)
        {
            d = va_arg(*p_ap, unsigned int);
            d_len = len_u(d);
        }
        else
        {
            if((d = va_arg(*p_ap, int)) < 0)
                neg = 1;
            d_len = len_int(d);
        }
//ja capturou, segundo a logica do complemento a 2. Eh o q de fato sera apresentado OK

        if (f->point) //se tiver precisao, quem conta eh ela (p efeito de 0)
        {
            zero = f->precision - d_len;
            if (zero > 0)
                space = f->width - f->precision;
            else
                space = f->width - d_len;
            if(neg)
                space--;
            if(!(f->flagminus))//alinhado a direita
            {
                ft_printspacezero( 1, space, p);
                if (neg)
                    ft_putchar('-', p);
                if(zero > 0)
                    ft_printspacezero( 0, zero, p);
                if(signal)
                    ft_putnbr(d, p);
                else
                    ft_putnbr_u(d, p);
            }
            else
            {
                if (neg)
                    ft_putchar('-', p);
                if(zero > 0)
                    ft_printspacezero( 0, zero, p);
                if(signal)
                    ft_putnbr(d, p);
                else
                    ft_putnbr_u(d, p);
                ft_printspacezero( 1, space, p);
            }
        }
        else
        {
            if(neg)
                d_len++;
            space = f->width - d_len;
            zero = space;
            if(f->flagzero)
            {
                if(neg)
                    ft_putchar('-', p);
                ft_printspacezero(0, zero, p);
                if(signal)
                    ft_putnbr(d, p);
                else
                    ft_putnbr_u(d, p);
                
            }
            else
            {
                if(f->flagminus)
                {
                    if(neg)
                        ft_putchar('-', p);
                    if(signal)
                        ft_putnbr(d, p);
                    else
                        ft_putnbr_u(d, p);
                    ft_printspacezero(1, space, p);
                }
                else
                {
                    ft_printspacezero(1, space, p);
                    if(neg)
                        ft_putchar('-', p);
                    if(signal)
                        ft_putnbr(d, p);
                    else
                        ft_putnbr_u(d, p);
                }
            }
        }
}


/* STRING */
static int      strlen(char *s) 
{
    int count;
    //if(!s)
    //    return(6); //(null)
    count = 0;
    while(*s)
    {
        count++;
        s++;
    }
    return(count);
}
/*
static char*    strcpy(char *s, int qtt) //CABECALHOOOOOOOO
{
    int i;
    char *t;

    if(!(t = malloc((qtt + 1)*sizeof(char))))
        return(NULL);

    i = 0;
    while(i < qtt)
    {
        t[i] = s[i];
        i++;
    }
    t[i] = '\0';
    return(t);
}
*/

static void     ft_printstr(va_list *p_ap, int *p, struct fields *f) 
{ 
        char *s;
        char *t;
        int str_len;
        int space;
        int qtt;
        
        s = va_arg(*p_ap, char *);
        if(!s)
        {
            //s = malloc( 7 * sizeof(char));
            s = "(null)";
        } 
        str_len = strlen(s);
        qtt = str_len;

        if (f->point) //se tiver precisao, quem conta eh ela (p efeito de 0)
        {
            if(f->precision < str_len)
            {
                //t = strcpy(s,f->precision);
                qtt = f->precision;
                space = f->width - f->precision;
                //use_t = 1;
            }
            else
                space = f->width - str_len;
        }
        else
            space = f->width - str_len;
        if(!(f->flagminus))//alinhado a direita
        {
            ft_printspacezero( 1, space, p);
            ft_putstr(s, p, qtt);
        }
        else
        {
            ft_putstr(s, p, qtt);
            ft_printspacezero( 1, space, p);
        }
}
/* CHAR */
static void     ft_printchar(va_list *p_ap, int *p, struct fields *f)
{ 
    //Nao faz sentido precisao nem flag zero!
    //fazer fillspecifier rejeitar. Ela já é uma static int: Usar ela!
        char c;
        int space;

        space = f->width - 1;
        c = (char)va_arg(*p_ap, int);
        if(f->flagminus)
        {
            ft_putchar(c, p);
            ft_printspacezero(1, space, p);
        }
        else
        {
            ft_printspacezero(1, space, p);
            ft_putchar(c, p);
        }
}
static void     ft_strformat_init(struct fields *f)
{
    f->flagminus = 0;
    f->flagzero = 0;
    f->width = 0;
    f->point = 0;
    f->precision = 0;
    f->specifier = '\0';

}
static int     ft_fillflags(const char *fmt, int *fmt_inc, struct fields *f)
{
    int count_minus;
    int count_zero;
    count_minus = 0;
    count_zero = 0;
    while(*fmt == '-' || *fmt == '0')
    {
        if(*fmt == '-' && count_minus == 0)
        {
            f->flagminus = 1;
            count_minus++;
        }
        if(*fmt == '0' && count_zero == 0)
        {
            f->flagzero = 1;
            count_zero++;
        }
        if(count_minus && count_zero)
            return(-1);
        fmt++;
        (*fmt_inc)++;
    }
    return (0);

}
static int      ft_getnumber(const char *fmt, int *fmt_inc)
{
    int number;

    number = 0;
    if(*fmt < 58 && *fmt > 47)
    {
        number = *fmt - 48;
        fmt++;
        (*fmt_inc)++;
        while(*fmt < 58 && *fmt > 47)
        {
            number = number*10 + (*fmt - 48);
            fmt++;
            (*fmt_inc)++;
        }
    }
            
    return (number);
}
static void     ft_fillwidth(va_list *p_ap, const char *fmt,int *fmt_inc,struct fields *f)
{
    if(*fmt == '.')
    {
        f->point = 1;
        f->width = 0;
        fmt++;
        (*fmt_inc)++;
    }
    else
    {
        if (*fmt == '*')
        {
            f->width = va_arg(*p_ap, int);
            fmt++;
            (*fmt_inc)++;
        }
        else
        {
            f->width = ft_getnumber(fmt,fmt_inc);
            fmt = fmt + *fmt_inc;
        }
        if(*fmt == '.')
        {   
            f->point = 1;
            (*fmt_inc)++;
        }
        
    }
}
static int      ft_fillprecision(va_list *p_ap, const char *fmt, int *fmt_inc, struct fields *f)
{
    if(*fmt == '-')
        return (-1);
    else if (f->point)
    {
        if(*fmt == '*')
        {
            f->precision = va_arg(*p_ap, int);
            (*fmt_inc)++;
        }
        else
            f->precision = ft_getnumber(fmt,fmt_inc);
        return (0);
    }
    else
        return (0);
}
static int      ft_fillspecifier(const char *fmt, int *fmt_inc, struct fields *f)
{
    if( *fmt != 's' && *fmt != 'c' && *fmt != 'i' && *fmt != 'u' && *fmt != 'd' && *fmt != 'x' && *fmt != 'X' && *fmt != 'p')
        return (-1);
    else
    {
        f->specifier = *fmt;
        //
        //(if specifier = p e existe flah 0 ou precisao: return -1, etc)
        //
        //AQUI VAI CONTROLE DE FLAGS ----erros de input
        //
        //
        (*fmt_inc)++;
        return (0);
    }
}



static int     ft_fieldstorage(va_list *p_ap,const char *fmt,int *fmt_inc, struct fields *f)
{
    /*
     * Eh preciso corrigit fmt na ft_printf 
     * pq aqui caminha-se nela
     *
     */
    int fmt_acc;
    *fmt_inc = 0; //fmt n foi incrementada ainda
    if(ft_fillflags(fmt,fmt_inc,f) == -1) //-------------chama 1 funcao
        return(-1);
    fmt += *fmt_inc; //corrige referencia
    fmt_acc = *fmt_inc;//acumula p devolver p printf
    *fmt_inc = 0;
    ft_fillwidth(p_ap,fmt,fmt_inc,f);         //---------------chama 2 funcao   
    fmt += *fmt_inc; //corrige referencia
    fmt_acc += *fmt_inc;//acumula p devolver p printf
    if (f->point)
    {
        *fmt_inc = 0;
        if(ft_fillprecision(p_ap, fmt, fmt_inc, f) == -1) //----- + 1 funcao
            return (-1);
        fmt += *fmt_inc; //corrige referencia
        fmt_acc += *fmt_inc;//acumula p devolver p printf
    }
    *fmt_inc = 0;
    ft_fillspecifier(fmt, fmt_inc, f);   //------------- + 1 funcao
    fmt += *fmt_inc; //corrige referencia
    fmt_acc += *fmt_inc;//acumula p devolver p printf
    *fmt_inc = fmt_acc; 
    return (0);
    
}

/* CHOICE C S P D I U X */
static void     ft_specifier_redirect(va_list *p_ap, char sp, int *p, struct fields *f)
{
    if(sp == 'd' || sp == 'i') //OK
        ft_printint(p_ap, 1, p,f); 
    if(sp == 'u') //OK
        ft_printint(p_ap, 0, p,f); 
    else if(sp == 'c') //OK
        ft_printchar(p_ap,p, f); 
    else if(sp == 's') //OK
        ft_printstr(p_ap,p, f); 
    else if(sp == 'x') //OK
        ft_printhex(p_ap, 0, p, f);
    else if(sp == 'X') //OK
        ft_printhex(p_ap, 1, p, f);
    else if(sp == 'p') //OK
        ft_printpointer(p_ap, p, f);
    /*
    */
}

/*
 * ***************************************
 * PRINTF
 * ****************************************
 */
int             ft_printf(const char *fmt, ...)
{
    int printed;
    int fmt_inc;
    va_list ap;
    struct fields *strformat;
    
    if(!fmt)
        return(-1);
    printed = 0;
    strformat = (struct fields*)malloc(sizeof(struct fields));
    va_start(ap, fmt);
    while (*fmt)
    {
        if(*fmt != '%')
        {
            ft_putchar(*fmt, &printed);
            fmt++;
        }
        else
        {
            fmt++;
            if(*fmt == '%')
            {
                ft_putchar(*fmt, &printed);
                fmt++;
            }
            else
            {
                ft_strformat_init(strformat);
                if(ft_fieldstorage(&ap, fmt,&fmt_inc,strformat) == -1)
                {
                    free(strformat);
                    return (-1);
                }
                ft_specifier_redirect(&ap, strformat->specifier, &printed, strformat);
                fmt = fmt + fmt_inc;

               /* TO DEBUG FLAG STORAGE
                *
                printf("\nminus( %d ) ", strformat->flagminus);
                printf("zero( %d ) ",strformat->flagzero);
                printf("width( %d ) ",strformat->width);
                printf("point( %d ) ",strformat->point);
                printf("precision( %d ) ", strformat->precision);
                printf("specifier( %c )\n\n", strformat->specifier);
                *
                */
            }
        }
    }
    va_end(ap);

    free(strformat);
    return (printed);
}
/*
 * ***************************************
 * MAIN
 * ****************************************
 */
int            main()
{
    int qtt;
    char c = 'C';
    char d = '\0';
    char *p;
    char *q;
    char *s = "alo vc!!";
    char *v;


    p = &c;
    q = &d;


    printf("\n\n\n");
    qtt = printf("|%-16.4s|\n", s);
    printf("%d caracteres impressos\n\n", qtt);
    qtt = ft_printf("|%-16.4s|\n", s);
    printf("%d caracteres impressos\n\n", qtt);

    printf("\n\n\n");
    qtt = printf("|%-16p|\n", &d);
    printf("%d caracteres impressos\n\n", qtt);
    qtt = ft_printf("|%-16p|\n", &d);
    printf("%d caracteres impressos\n\n", qtt);

    //printf("|%-42.23s|\n", "alo vc!!!");


    return (0);
}

