#ifndef __DEV_DRIVER_H_
#define __DEV_DRIVER_H_

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/delay.h>

#include <linux/interrupt.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/workqueue.h>

#include "char_table.h"
#include "fpga_dot_font.h"

/* predefined major number */
#define MAJOR_NUMBER 242

/* device name */
#define DEVICE_NAME "dev_driver"

/* module information */
static int major = MAJOR_NUMBER;
static struct class *cls;

/* functions prototype */
static int device_open (struct inode *inode, struct file *file);
static int device_release (struct inode *inode, struct file *file);
static int device_ioctl (struct file *, unsigned long command, unsigned long arg);

/* define file operations */
static struct file_operations fops =
{
	.owner					=	THIS_MODULE,
	.open						=	device_open,
	.release				=	device_release,
  .unlocked_ioctl	= device_ioctl,
};

/* ioctl command */
#define GET_TEXT          _IOR(MAJOR_NUMBER, 0, unsigned long)
#define GET_NUMBER        _IOR(MAJOR_NUMBER, 1, unsigned long)
#define SET_FND           _IOW(MAJOR_NUMBER, 2, unsigned long)
#define SET_DOT           _IOW(MAJOR_NUMBER, 3, unsigned long)
#define SET_LCD_FIRST     _IOW(MAJOR_NUMBER, 4, unsigned long)
#define SET_LCD_SECOND    _IOW(MAJOR_NUMBER, 5, unsigned long)
#define COUNT_WITH_BLOCK  _IOW(MAJOR_NUMBER, 6, unsigned long)
#define COUNT_NONBLOCK    _IOW(MAJOR_NUMBER, 7, unsigned long)
#define CLEAR_DEVICE			_IO(MAJOR_NUMBER, 8)

#define GPIO_HOME_BUTTON IMX_GPIO_NR(1,11)
#define GPIO_BACK_BUTTON IMX_GPIO_NR(1,12)
#define NAME_HOME_BUTTON "home"
#define NAME_PAUSE_BUTTON "back"

#define FALLING_EDGE 0
#define RISING_EDGE  1

typedef struct {
	struct work_struct work;
	struct task_struct *thr_id;
} work_t;

work_t *work;
static struct workqueue_struct *workq;

wait_queue_head_t waitq;
DECLARE_WAIT_QUEUE_HEAD(waitq);

irqreturn_t home_button_handler (int irq, void* dev_id, struct pt_regs* reg);
void stop_producer (struct work_struct *work);

void push_wait_queue (void);
void pop_wait_queue (void);

struct task_struct *str_producer_id = NULL;
struct task_struct *num_producer_id = NULL;

int produce_number (void *arg);
int produce_string (void *arg);

static struct timer_list timer;

void timer_func (unsigned long arg);
void set_timer (void* data);

/* device physical addresses */
#define FND_ADDR    0x08000004
#define LCD_ADDR    0x08000090
#define DOT_ADDR    0x08000210
#define SWITCH_ADDR 0x08000050

/* define io data size */
#define LCD_BUFFER_SIZE 32
#define SWTICH_BUTTON   9
#define DOT_FONT_NUM    9

/* mapped addresses of IO devices */
static unsigned char *fnd_addr;
static unsigned char *lcd_addr;
static unsigned char *dot_addr;
static unsigned char *switch_addr;

/* io control functions */
void map_io_dev (void);
void unmap_io_dev (void);

/* functions that control output devices */
void set_fnd (int value);
int get_fnd (void);
int read_switch (void);
void set_dot (int value);
int write_lcd (char *buf, int start, int length);

#endif