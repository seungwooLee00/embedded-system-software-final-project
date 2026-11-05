#include <linux/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

/* predefined major number */
#define MAJOR_NUMBER 242

/* device name */
#define DEVICE_NAME "/dev/dev_driver"

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

char*
fpgaReadText (void) {
  char* text = malloc (sizeof(char) * 17);
  int fd = open (DEVICE_NAME, O_RDONLY);
  int ret = ioctl (fd, GET_TEXT, text);
  close (fd);

  if (ret > 0) {
    return text;
  }
  else {
    free (text);
    return NULL;
  }
}

int 
fpgaReadNumber (void) {
  int num;
  int fd = open (DEVICE_NAME, O_RDONLY);
  int ret = ioctl (fd, GET_NUMBER, &num);
  close (fd);

  if (ret > 0) {
    return num;
  }
  else {
    return -1;
  }
}

void 
fpgaSetLcdFirst (char *msg) {
  int fd;
  char buf[17];
  int len;

  strncpy (buf, msg, 16);
  len = strlen (buf);
  memset (buf+len, ' ', 16-len);
  buf[16] = '\0';

  fd = open (DEVICE_NAME, O_WRONLY);
  ioctl (fd, SET_LCD_FIRST, buf);
  close (fd);
}

void 
fpgaSetLcdSecond (char *msg) {
  int fd;
  char buf[17];
  int len;

  strncpy (buf, msg, 16);
  len = strlen (buf);
  memset (buf+len, ' ', 16-len);
  buf[16] = '\0';

  fd = open (DEVICE_NAME, O_WRONLY);
  ioctl (fd, SET_LCD_SECOND, buf);
  close (fd);
}

void 
fpgaSetFnd (int value) {
  if (value < 0 || value > 9999) return ;

  int data = value;
  int fd = open (DEVICE_NAME, O_WRONLY);
  ioctl (fd, SET_FND, &data);
  close (fd);
}

void 
fpgaSetDot (int value) {
  if (value > 10) return ;

  int data = value;
  int fd = open (DEVICE_NAME, O_WRONLY);
  ioctl (fd, SET_DOT, &data);
  close (fd);
}

void 
fpgaCountNonBlock (int sec) {
  if (sec >= 10) return ;
  
  int data = sec;
  int fd = open (DEVICE_NAME, O_RDWR);
  ioctl (fd, COUNT_NONBLOCK, &data);
  close (fd);
}

void 
fpgaCountWithBlock (int sec) {
  if (sec >= 10) return ;
  
  int data = sec;
  int fd = open (DEVICE_NAME, O_RDWR);
  ioctl (fd, COUNT_WITH_BLOCK, &data);
  close (fd);
}

void
fpgaClearDevice (void) {
  int fd = open (DEVICE_NAME, O_RDWR);
  ioctl (fd, CLEAR_DEVICE);
  close (fd);
}

int main (void) {
  
  fpgaSetFnd (1004);
  sleep (2);

  fpgaSetDot (7);
  sleep (2);

  printf ("user: %d\n", fpgaReadNumber ());
  printf ("user: %s\n", fpgaReadText ());

  fpgaSetLcdFirst ("hello world!");
  sleep (2);
  fpgaSetLcdSecond ("leeseungwoo");
  sleep (2);

  fpgaCountWithBlock (9);
  fpgaClearDevice ();

  return 0;
}