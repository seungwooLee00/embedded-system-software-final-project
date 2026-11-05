#include "dev_driver.h"

/* ******************************************************************************************* */

static int
device_ioctl (struct file *file, unsigned long command, unsigned long arg) {
  int val;
  int ret;
  char *buf;

  switch (command) {

    case GET_TEXT:
      pr_info ("ioctl get string");
      if (str_producer_id == NULL) {
        buf = kmalloc (17, GFP_KERNEL);
        memset (buf, ' ', 16);

        workq = create_workqueue("work_queue");
        str_producer_id = (struct task_struct*) kthread_run (produce_string, (void *) buf, "string");

        push_wait_queue ();
        str_producer_id = NULL;
        pr_info ("wake up");

        copy_to_user ((char *)arg, buf, 17);
        pr_info ("string: %s", buf);
        kfree (buf);
        return 16;
      }
      else return -1;

    case GET_NUMBER:
      pr_info ("ioctl get number");
      if (num_producer_id == NULL) {
        workq = create_workqueue("work_queue");
        num_producer_id = (struct task_struct*) kthread_run (produce_number, (void *)&val, "number");

        push_wait_queue ();
        pr_info ("wake up");
        num_producer_id = NULL;

        copy_to_user ((int *) arg, &val, sizeof (int));
        pr_info ("number: %d", val);
      }
      return 1;
    
    case SET_FND:
      copy_from_user (&val, (int *) arg, sizeof(int));
      set_fnd (val);
      return 1;
    
    case SET_DOT:
      copy_from_user (&val, (int *) arg, sizeof(int));
      set_dot (val);
      return 1;
    
    case SET_LCD_FIRST:
      buf = kmalloc (16, GFP_KERNEL);
      copy_from_user (buf, (char *) arg, 16);
      write_lcd (buf, 0, 16);
      kfree (buf);
      return 1;

    case SET_LCD_SECOND:
      buf = kmalloc (16, GFP_KERNEL);
      copy_from_user (buf, (char *) arg, 16);
      write_lcd (buf, 16, 16);
      kfree (buf);
      return 1;

    case COUNT_WITH_BLOCK:
      copy_from_user (&val, (int *) arg, sizeof (int));
      init_timer (&timer);
      set_timer (&val);
      push_wait_queue ();
      del_timer_sync (&timer);
      return 1;

    case COUNT_NONBLOCK:
      return -1;

    case CLEAR_DEVICE:
      set_fnd (0);
      set_dot (-1);
      buf = kmalloc (32, GFP_KERNEL);
      memset (buf, ' ', 32);
      write_lcd (buf, 0, 32);
      return 1;

    default:
      return -1;
  }
}

int __init
device_init (void) {

  int res;

  /* register device (static allocation) */
  res = register_chrdev (major, DEVICE_NAME, &fops);

  if (res < 0) {
    pr_alert ("Restering device fail with %d\n", res);
    return res;
  }

  /* make device file */
  cls = class_create (THIS_MODULE, DEVICE_NAME);
  device_create (cls, NULL, MKDEV (major, 0), NULL, DEVICE_NAME);
  pr_info ("Major Number: %d\nDevice Name: /dev/%s\n", major, DEVICE_NAME);

  /* map io devices */
  map_io_dev ();
  
  return 0;
}

void __exit
device_exit (void) {

  /* deallocate io device */
  unmap_io_dev ();

  /* delete device file */
  device_destroy (cls, MKDEV (major, 0));
  class_destroy (cls);

  /* unregister device */
  unregister_chrdev (major, DEVICE_NAME);

  pr_info ("device exit\n");
}

static int
device_open( struct inode *inode, struct file *file) {
  int ret, irq;

  gpio_direction_input(GPIO_HOME_BUTTON);
  irq = gpio_to_irq(GPIO_HOME_BUTTON);
  ret = request_irq(irq, home_button_handler, IRQF_TRIGGER_RISING, NAME_HOME_BUTTON, 0);

  pr_info ("device opened\n");
  return 0;
}

static int
device_release (struct inode *inode, struct file *file) {

  free_irq(gpio_to_irq(GPIO_HOME_BUTTON), NULL);
  pr_info ("device closed\n");

  return 0;
}

int __init device_init (void);
void __exit device_exit (void);

module_init (device_init);
module_exit (device_exit);

MODULE_LICENSE("GPL");

/* ******************************************************************************************* */



/* ******************************************************************************************* */

void
set_fnd (int value) {
	unsigned short int value_short = 0;
  pr_info ("set fnd %d", value);
  mdelay (200);
  value_short = (value/1000)%10 << 12 | (value/100)%10 << 8 | (value/10)%10 << 4 | value%10;
  outw(value_short,(unsigned int) fnd_addr);
}

int 
get_fnd (void) {
  int ret = 0;
	unsigned short int value = 0;
  mdelay (200);
  value = inw((unsigned int) fnd_addr);
  ret += ((value >> 12) & 0xF) * 1000;
  ret += ((value >> 8) & 0xF) * 100;
  ret += ((value >> 4) & 0xF) * 10;
  ret += value & 0xF;

  pr_info ("get fnd %d", ret);
	return ret;
}

int
read_switch (void) {
  int i;
  unsigned short int value;

  mdelay (200);
  for (i=0; i<SWTICH_BUTTON; i++) {
    value = inw ((unsigned int) switch_addr + i*2);
    value &= 0xFF;
    if (value == 1) {
      pr_info ("switch %d", i+1);
      return i+1;
    }
    else if (value > 1) return 0;
  }
  return -1;
}

void 
set_dot (int value) {
	int i;
	unsigned char *dot_font;
	unsigned short int _s_value;

  if (value < 0) dot_font = fpga_set_blank;
  else if (value < 10) dot_font = fpga_number[value];
  else if (value == 10) dot_font = fpga_set_full;
  else return;
  
  mdelay (200);
  pr_info ("dot %d", value);
	for(i=0; i<=DOT_FONT_NUM; i++) {
    _s_value = dot_font[i] & 0x7F;
		outw (_s_value, (unsigned int) dot_addr+i*2);
  }
}

int 
write_lcd (char *buf, int start, int length) {
  int i, j;
  unsigned short int _s_value = 0;

  if (start+length > 32) return -1;

  mdelay (200);
  pr_info ("lcd: %s", buf);
	for(i=start, j=0; j<length-1; i += 2, j += 2) {
    _s_value = (buf[j] & 0xFF) << 8 | (buf[j + 1] & 0xFF);
		outw(_s_value, (unsigned int) lcd_addr + i);
  }

	return length;
}

void
map_io_dev (void) {
  fnd_addr = ioremap (FND_ADDR, 0x04);
  lcd_addr = ioremap (LCD_ADDR, 0x32);
  dot_addr = ioremap(DOT_ADDR, 0x10);
  switch_addr = ioremap (SWITCH_ADDR, 0x18);
}

void
unmap_io_dev (void) {
  iounmap (lcd_addr);
  iounmap (fnd_addr);
  iounmap (dot_addr);
  iounmap (switch_addr);
}

/* ******************************************************************************************* */



/* ******************************************************************************************* */

irqreturn_t
home_button_handler (int irq, void* dev_id, struct pt_regs* reg) {
  int value = gpio_get_value (GPIO_HOME_BUTTON);

  if (value == RISING_EDGE) {
    pr_info ("home button pressed");
    
    if (num_producer_id != NULL) {
      work = (work_t *)kmalloc (sizeof (work_t), GFP_ATOMIC);
      if (work != NULL) {
        INIT_WORK((struct work_struct *) work, stop_producer);
        work->thr_id = num_producer_id;
        queue_work (workq, (struct work_struct *) work);
      }
    }
    else if (str_producer_id != NULL) {
      work = (work_t *)kmalloc (sizeof (work_t), GFP_ATOMIC);
      if (work != NULL) {
        INIT_WORK((struct work_struct *) work, stop_producer);
        work->thr_id = str_producer_id;
        queue_work (workq, (struct work_struct *) work);
      }
    }
    
  }
  return IRQ_HANDLED;
}

void
stop_producer (struct work_struct *work) {
  pr_info ("stop producer");

  work_t *_work = (work_t *) work;
  struct task_struct *thr_id = _work->thr_id;
  if (thr_id == NULL) {
    pr_info ("thread id is NULL");
    return ;
  }
  kthread_stop (thr_id);
  thr_id = NULL;
  kfree (work);
  pop_wait_queue ();
}

void
push_wait_queue (void) {
  interruptible_sleep_on (&waitq);
}

void pop_wait_queue (void) {
  wake_up_interruptible (&waitq);
}

/* ******************************************************************************************* */



/* ******************************************************************************************* */

int
produce_number (void *arg) {
  int ret, *num = (int *)arg;

  pr_info ("number producer starts");
  *num = 0;
  while (!kthread_should_stop ()) {
    mdelay (200);
    ret = read_switch ();
    if (ret >= 0) {
      *num = (*num)*10 + ret;
      set_fnd (*num);
    }
    if (*num >= 1000) break;
  }
  while (!kthread_should_stop ());

  return 1;
}

int
produce_string (void *arg) {
  int ret, cnt, len, prev;
  char *buf;
  int i;

  pr_info ("string producer starts");
  cnt = len = prev = 0;
  buf = (char *)arg;

  while (!kthread_should_stop ()) {
    mdelay (100);
    ret = read_switch ();
    if (ret < 0) continue;
    else if (ret == 0) {
      mdelay (100);
      set_dot (10);
      len++;
      cnt = 0;
      mdelay (100);
      set_dot (-1);
      if (len >= 16) break;
    }
    else {
      if (prev != ret) {
        cnt = 0;
        prev = ret;
      }
      buf[len] = char_table[ret][cnt];
      pr_info ("buf: %s", buf);
      write_lcd (buf, 16, 16);
      cnt++;
      if (cnt >= 3) cnt = 0;
    }
  }
  for (i=len+1; i<16; i++) {
    buf[i] = '$';
  }
  buf[16] = '\0';

  while (!kthread_should_stop ());
  return 1;
}

/* ******************************************************************************************* */



/* ******************************************************************************************* */

void
timer_func (unsigned long arg) {
  int *count = (int *)arg;
  
  if (*count > 0) {
    pr_info ("timer func %d", *count);
    set_dot (*count);
    (*count)--;
    set_timer (count);
  }
  else {
    pop_wait_queue ();
  }
}

void
set_timer (void* data) {
  timer.expires = get_jiffies_64 () + (HZ);
  timer.data = (unsigned long) data;
  timer.function = timer_func;
  add_timer (&timer);
}

/* ******************************************************************************************* */

