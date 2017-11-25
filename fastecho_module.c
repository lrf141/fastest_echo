#include <linux/module.h>
#include <linux/kernel.h>

//module info
MODULE_DESCRIPTION("The fastest echo server in kernel module");
MODULE_AUTHOR("lrf141");
MODULE_LICENSE("MIT");


static int fastecho_init_module(void){
        printk("Fastest Echo Server!!");
        return 0;
}


static void fastecho_cleanup_module(void){

        printk("Fastest Echo Server is unloaded!");

}


module_init(fastecho_init_module);
module_exit(fastecho_cleanup_module);