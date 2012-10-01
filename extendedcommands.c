#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <sys/wait.h>
#include <sys/limits.h>
#include <dirent.h>
#include <sys/stat.h>

#include <signal.h>
#include <sys/wait.h>

#include "common.h"
#include "cutils/properties.h"
#include "install.h"
//#include "make_ext4fs.h"
#include "minui/minui.h"
#include "minzip/DirUtil.h"
#include "roots.h"
#include "recovery_ui.h"

//#include "../../external/yaffs2/yaffs2/utils/mkyaffs2image.h"
//#include "../../external/yaffs2/yaffs2/utils/unyaffs.h"

#include "extendedcommands.h"
#include "nandroid.h"
#include "mounts.h"
//#include "flashutils/flashutils.h"
#include "edify/expr.h"
#include <libgen.h>
//#include "mtdutils/mtdutils.h"
//#include "bmlutils/bmlutils.h"
#include "cutils/android_reboot.h"

void show_help_menu()
{
	static char* items[] = { "About Recovery", "Touch Help",NULL };
    static char* headers[] = { "","About",NULL };
    for (;;)
    {
		gCurrentIcon = BACKGROUND_ICON_ABOUT;
        int chosen_item = get_menu_selection(headers, items, 0, 0);
        switch (chosen_item) {
			case 0:
				       //"Install a backup root (pro          ",
				ui_print("\n");
				ui_print("             [About]\n");
				ui_print("This project was made possible by a\n");
 				ui_print("locked bootloader,thanks to Motorola\n\n");
				ui_print("Recovery is based on CWM 5.5.0.4\n");
				ui_print("It is specialy rebuild for Motorola\n");
				ui_print("Spyder with a little tweak & added\n");
				ui_print("functions.\n\n");
				ui_print("Source code is made available at\n");
				ui_print("  https://github.com/whirleyes/\n");
				ui_print("\n");
				ui_print("        [Remarks and thanks]\n\n");
				ui_print("koush & CyanogenMod - CWM-R codes\n");
				ui_print("TeamWin - touch related item\n");
				ui_print("STS-Dev-Team - Spyder related conf\n");
				ui_print("koush & cvpcs - hijack binary codes\n");
				ui_print("tpruvot & CyanogenMod - ICS busybox\n");
				ui_print("dhacker29,koush,cvpcs - *Bootstrap\n");
				ui_print("Hashcode,Skrilax_CZ - 2nd-init stuff\n");
				ui_print("\n");
				ui_print("         [Special thanks]\n\n");
				ui_print("lukas77,dtrail1,leisures,sevenup30\n");
				ui_print("FrAsErTaG & core720 for ideas, \n");
				ui_print("support, bug report & testing.\n");
				break;
			case 1:
				ui_print("\n");		
				ui_print("\n");
				ui_print("          [TOUCH GESTURE]\n");
				ui_print("\n");
				ui_print("           hightlight up     \n");
				ui_print("                ^            \n");
				ui_print("        back  < + >  select  \n");
				ui_print("                v            \n");
				ui_print("          hightlight down    \n");
				ui_print("\n");
				ui_print("\n");
				ui_print("For safety reason, Touch to select\n");
				ui_print("is disable by default. It can be\n");
				ui_print("enabled in configuration\n");
            	break;
			default:
				return;
		}
    }
}

void show_safety_options_menu()
{
	static char* items[] = { "Install bootmenu hijack",
                             "Install su(root) to system",
                             "Install a backup root (protected)",
                             "Unroot (leave a protected backup)",
                             "Reset protection flag (all files)",
                              NULL
	};

    static char* headers[] = {  "","System protector & keeper",
                                NULL
    };
    
	if (!strcmp(recovery_mode, "second")) {
		items[0] = "Remove logwrapper hijack";
	}

    for (;;)
    {
		gCurrentIcon = BACKGROUND_ICON_SAFETY;
        int chosen_item = get_menu_selection(headers, items, 0, 0);
        switch (chosen_item)
        {
            case 0:
				ensure_path_mounted("/system");
				if (!strcmp(recovery_mode, "stock")) {
			    	ui_print("Installing hijack to /system...\n");
			    	if (0 != __system("/preinstall/bootmenu/script/install_hijack.sh")) {
			    		ui_print("Error installing!\n");
					} else {
			    		ui_print("Done installing!\n");
					}
				} else {
			    	ui_print("Removing hijack from /system...\n");
			    	if (0 != __system("/preinstall/bootmenu/script/remove_hijack.sh")) {
			    		ui_print("Error removing!\n");
					} else {
			    		ui_print("Done removing!\n");
					}
				}
				ensure_path_unmounted("/system");
                break;
            case 1:
				ensure_path_mounted("/system");
			    ui_print("Install root to /system...\n");
			    if (0 != __system("/preinstall/bootmenu/script/install_root.sh")) {
			    	ui_print("Error installing!\n");
				} else {
			    	ui_print("Done installing!\n");
				}			
				ensure_path_unmounted("/system");
				break;
			case 2:
				ensure_path_mounted("/system");
			    ui_print("Install a backup root to /system...\n");
			    if (0 != __system("/preinstall/bootmenu/script/install_backup_root.sh")) {
			    	ui_print("Error installing!\n");
				} else {
			    	ui_print("Done installing!\n");
				}			
				ensure_path_unmounted("/system");
                break;
			case 3:
				ensure_path_mounted("/system");
			    ui_print("Install a backup root to /system...\n");
			    if (0 != __system("/preinstall/bootmenu/script/install_backup_root.sh")) {
			    	ui_print("Error installing!\n");
					break;
				} else {
			    	ui_print("Done installing!\n");
				}
			    ui_print("Unrooting...\n");
			    __system("rm /system/bin/su");
			    __system("rm /system/xbin/su");
				ui_print("Done unrooting!\n");
				ensure_path_unmounted("/system");
                break;
			case 4:
				ensure_path_mounted("/system");
			    ui_print("Removing any immutable flag in /system...\n");
			    __system("chattr -i -R /system/");
				ui_print("Done!\n");
				ensure_path_unmounted("/system");
                break;
            default:
				return;
        }
    }
}

int show_power_options_menu()
{
	static char* items[] = { "Reboot",
                             "Power off",
                             "Reboot to 1st-system",
                             "Reboot to 2nd-system",
                             "Reboot to stock recovery",
                             "Reboot to bootloader",
                              NULL
	};

    static char* headers[] = { "","Reboot options", NULL };
    
	gCurrentIcon = BACKGROUND_ICON_POWER;
    int chosen_item = get_menu_selection(headers, items, 0, 0);
    switch (chosen_item) {
    	case 0:
		case 1:
		case 2:
    	case 3:
		case 4:
		case 5:
			return chosen_item;
		default:
			return 6;
    }
}

int signature_check_enabled = 1;
int script_assert_enabled = 1;
static const char *SDCARD_UPDATE_FILE = "/sdcard/update.zip";

void
toggle_signature_check()
{
    signature_check_enabled = !signature_check_enabled;
    ui_print("Signature Check: %s\n", signature_check_enabled ? "Enabled" : "Disabled");
}

void toggle_script_asserts()
{
    script_assert_enabled = !script_assert_enabled;
    ui_print("Script Asserts: %s\n", script_assert_enabled ? "Enabled" : "Disabled");
}

/// v 0.2.7
void write_sys_txt(const char* file, const char* value) {
	FILE* f = fopen(file, "w");
	if (f != NULL) {
    	fprintf(f, "%s", value);
    	fclose(f);
  	}
}

void show_choose_radio_menu(const char *mount_point) {
    if (ensure_path_mounted(mount_point) != 0) {
        LOGE ("Can't mount %s\n", mount_point);
        return;
    }

    static char* headers[] = {  "","Choose an image to flash", NULL };
    char* file = choose_file_menu(mount_point, ".img", headers);
    if (file == NULL)
        return;
    static char* confirm_install  = "Confirm flash?";
    static char confirm[PATH_MAX];
    sprintf(confirm, "Yes - Install %s", basename(file));
    if (confirm_selection(confirm_install, confirm)) {
		property_set("firmware.path", file);
		write_sys_txt("/sys/bus/usb/devices/usb1/power/control", "on");
		write_sys_txt("/sys/bus/usb/devices/usb2/power/control", "on");
		
        ui_print("\n-- Flashing: %s\n", file);
		int status = install_package("/preinstall/bootmenu/script/radio.zip");

		ui_reset_progress();
		write_sys_txt("/sys/bus/usb/devices/usb1/power/control", "auto");
		write_sys_txt("/sys/bus/usb/devices/usb2/power/control", "auto");
		property_set("firmware.path", "");

  		if (status != INSTALL_SUCCESS) {
        	ui_print("Flashing aborted.\n");
			static char* headers[] = {  "","Flashing aborted.",NULL };
    		static char* list[] = { NULL };
			ui_set_background(BACKGROUND_ICON_FIRMWARE_ERROR);
        	int chosen_item = get_menu_selection(headers, list, 0, 0);
   	    	if (chosen_item == GO_BACK || chosen_item == 0) {
   	    	    return;
			}
   		}
    	ui_print("\nFlashing %s completed.\n", basename(file));
	}
}

void show_install_radio_menu() {
	char* headers[] = {  "","Flash radio firmware", "", "internal storage", NULL  };
	char* menu[] = {  "Choose bp.img,lte.img or cefs.img", "Change firmware file location", NULL };

	char location[PATH_MAX] = "";
	char title[PATH_MAX] = "";	
	get_title(1,location,title);
    for (;;) {
		headers[3] = title;
		gCurrentIcon = BACKGROUND_ICON_FIRMWARE_INSTALLING;
        int chosen_item = get_menu_selection(headers, menu, 0, 0);
        switch (chosen_item) {
            case 0:
                show_choose_radio_menu(location);
                break;
            case 1:
				show_path_chooser(1,location,title);
				break;
            default:
                return;
        }

    }
}

///

int install_zip(const char* packagefilepath)
{
    ui_print("\n-- Installing: %s\n", packagefilepath);
/*    if (device_flash_type() == MTD) {
        set_sdcard_update_bootloader_message();
    }*/
    int status = install_package(packagefilepath);
    ui_reset_progress();
    if (status != INSTALL_SUCCESS) {
        ui_print("Installation aborted.\n");
		static char* headers[] = {  "","Installation aborted.",NULL };
    	static char* list[] = { NULL };
		ui_set_background(BACKGROUND_ICON_ERROR);
        int chosen_item = get_menu_selection(headers, list, 0, 0);
   	    if (chosen_item == GO_BACK || chosen_item == 0) {
   	        return 1;
		}
   	}
    ui_print("\nInstall from zip complete.\n");
   	return 0;
}
//                              "choose zip from internal sdcard",
char* INSTALL_MENU_ITEMS[] = {  "Install a zip package",
                                "Apply an update.zip",
                                "Toggle signature verification",
                                "Toggle script asserts",
                                "Change zip file location",
                                NULL };

void show_install_update_menu()
{
    static char* headers[] = {  "","Apply update from a zip file","","internal storage", NULL  };
 	char location[PATH_MAX] = "";
	char title[PATH_MAX] = "";	
	get_title(1,location,title);
    for (;;) {
		headers[3] = title;
		gCurrentIcon = BACKGROUND_ICON_ZIP;
        int chosen_item = get_menu_selection(headers, INSTALL_MENU_ITEMS, 0, 0);
        switch (chosen_item) {
            case 0:
                show_choose_zip_menu(location);
                break;
            case 1:
            {
                if (confirm_selection("Confirm install?", "Yes - Install an update.zip")) {
					sprintf(location + strlen(location), "/update.zip");
                    install_zip(location);
				}
                break;
            }
            case 2:
                toggle_signature_check();
                break;
            case 3:
                toggle_script_asserts();
                break;
			case 4:
				show_path_chooser(1,location,title);
				break;
            default:
                return;
        }

    }
}

void free_string_array(char** array)
{
    if (array == NULL)
        return;
    char* cursor = array[0];
    int i = 0;
    while (cursor != NULL)
    {
        free(cursor);
        cursor = array[++i];
    }
    free(array);
}

char** gather_files(const char* directory, const char* fileExtensionOrDirectory, int* numFiles)
{
    char path[PATH_MAX] = "";
    DIR *dir;
    struct dirent *de;
    int total = 0;
    int i;
    char** files = NULL;
    int pass;
    *numFiles = 0;
    int dirLen = strlen(directory);

    dir = opendir(directory);
    if (dir == NULL) {
        ui_print("Couldn't open directory.\n");
        return NULL;
    }

    int extension_length = 0;
    if (fileExtensionOrDirectory != NULL)
        extension_length = strlen(fileExtensionOrDirectory);

    int isCounting = 1;
    i = 0;
    for (pass = 0; pass < 2; pass++) {
        while ((de=readdir(dir)) != NULL) {
            // skip hidden files
            if (de->d_name[0] == '.')
                continue;

            // NULL means that we are gathering directories, so skip this
            if (fileExtensionOrDirectory != NULL)
            {
                // make sure that we can have the desired extension (prevent seg fault)
                if (strlen(de->d_name) < extension_length)
                    continue;
                // compare the extension
                if (strcmp(de->d_name + strlen(de->d_name) - extension_length, fileExtensionOrDirectory) != 0)
                    continue;
            }
            else
            {
                struct stat info;
                char fullFileName[PATH_MAX];
                strcpy(fullFileName, directory);
                strcat(fullFileName, de->d_name);
                stat(fullFileName, &info);
                // make sure it is a directory
                if (!(S_ISDIR(info.st_mode)))
                    continue;
            }

            if (pass == 0)
            {
                total++;
                continue;
            }

            files[i] = (char*) malloc(dirLen + strlen(de->d_name) + 2);
            strcpy(files[i], directory);
            strcat(files[i], de->d_name);
            if (fileExtensionOrDirectory == NULL)
                strcat(files[i], "/");
            i++;
        }
        if (pass == 1)
            break;
        if (total == 0)
            break;
        rewinddir(dir);
        *numFiles = total;
        files = (char**) malloc((total+1)*sizeof(char*));
        files[total]=NULL;
    }

    if(closedir(dir) < 0) {
        LOGE("Failed to close directory.");
    }

    if (total==0) {
        return NULL;
    }

    // sort the result
    if (files != NULL) {
        for (i = 0; i < total; i++) {
            int curMax = -1;
            int j;
            for (j = 0; j < total - i; j++) {
                if (curMax == -1 || strcmp(files[curMax], files[j]) < 0)
                    curMax = j;
            }
            char* temp = files[curMax];
            files[curMax] = files[total - i - 1];
            files[total - i - 1] = temp;
        }
    }

    return files;
}

// pass in NULL for fileExtensionOrDirectory and you will get a directory chooser
char* choose_file_menu(const char* directory, const char* fileExtensionOrDirectory, const char* headers[])
{
    char path[PATH_MAX] = "";
    DIR *dir;
    struct dirent *de;
    int numFiles = 0;
    int numDirs = 0;
    int i;
    char* return_value = NULL;
    int dir_len = strlen(directory);

    char** files = gather_files(directory, fileExtensionOrDirectory, &numFiles);
    char** dirs = NULL;
    if (fileExtensionOrDirectory != NULL)
        dirs = gather_files(directory, NULL, &numDirs);
    int total = numDirs + numFiles;
    if (total == 0)
    {
        ui_print("No files found.\n");
    }
    else
    {
        char** list = (char**) malloc((total + 1) * sizeof(char*));
        list[total] = NULL;


        for (i = 0 ; i < numDirs; i++)
        {
            list[i] = strdup(dirs[i] + dir_len);
        }

        for (i = 0 ; i < numFiles; i++)
        {
            list[numDirs + i] = strdup(files[i] + dir_len);
        }

        for (;;)
        {
            int chosen_item = get_menu_selection(headers, list, 0, 0);
            if (chosen_item == GO_BACK)
                break;
            static char ret[PATH_MAX];
            if (chosen_item < numDirs)
            {
                char* subret = choose_file_menu(dirs[chosen_item], fileExtensionOrDirectory, headers);
                if (subret != NULL)
                {
                    strcpy(ret, subret);
                    return_value = ret;
                    break;
                }
                continue;
            }
            strcpy(ret, files[chosen_item - numDirs]);
            return_value = ret;
            break;
        }
        free_string_array(list);
    }

    free_string_array(files);
    free_string_array(dirs);
    return return_value;
}

void show_choose_zip_menu(const char *mount_point)
{
    if (ensure_path_mounted(mount_point) != 0) {
        LOGE ("Can't mount %s\n", mount_point);
        return;
    }

    static char* headers[] = {  "","Choose a zip to apply",
                                NULL
    };

    char* file = choose_file_menu(mount_point, ".zip", headers);
    if (file == NULL)
        return;
    static char* confirm_install  = "Confirm install?";
    static char confirm[PATH_MAX];
    sprintf(confirm, "Yes - Install %s", basename(file));
    if (confirm_selection(confirm_install, confirm))
        install_zip(file);
}

void show_nandroid_restore_menu(const char* path)
{
    if (ensure_path_mounted(path) != 0) {
        LOGE("Can't mount %s\n", path);
        return;
    }

    static char* headers[] = {  "","Choose an image to restore",
                                NULL
    };

    char tmp[PATH_MAX];
    sprintf(tmp, "%s/clockworkmod/backup/", path);
    char* file = choose_file_menu(tmp, NULL, headers);
    if (file == NULL)
        return;

    if (confirm_selection("Confirm restore?", "Yes - Restore")) {
		if (!strcmp(recovery_mode, "stock")) {
			nandroid_restore(file, 1, 1, 1, 1, 1, 1);
		} else {
			nandroid_restore(file, 1, 1, 1, 1, 0, 1);
		}
	}
}

//#ifndef BOARD_UMS_LUNFILE
//#define BOARD_UMS_LUNFILE	"/sys/devices/platform/usb_mass_storage/lun0/file"
//#endif
//#define BOARD_UMS_LUNFILE2 "/sys/devices/platform/usb_mass_storage/lun1/file"

//kernel dependant
//#define BOARD_UMS_LUNFILE "/sys/class/android_usb/android0/f_mass_storage/lun0/file"
//#define BOARD_UMS_LUNFILE2 "/sys/class/android_usb/android0/f_mass_storage/lun1/file"

//omap specific
#define BOARD_UMS_LUNFILE "/sys/devices/platform/omap/musb-omap2430/musb-hdrc/gadget/lun0/file"
#define BOARD_UMS_LUNFILE2 "/sys/devices/platform/omap/musb-omap2430/musb-hdrc/gadget/lun1/file"

void show_mount_usb_storage_menu()
{
    int fd;
	int fd2;
    Volume *vol = volume_for_path("/sdcard");
	Volume *vol2 = volume_for_path("/emmc");

    if ((fd = open(BOARD_UMS_LUNFILE, O_WRONLY)) < 0) {
        LOGE("Unable to open ums lunfile (%s)", strerror(errno));
        return;
    }

    if ((write(fd, vol->device, strlen(vol->device)) < 0) &&
        (!vol->device2 || (write(fd, vol->device, strlen(vol->device2)) < 0))) {
        LOGE("Unable to write to ums lunfile (%s)", strerror(errno));
        close(fd);
        return;
    }

	if ((fd2 = open(BOARD_UMS_LUNFILE2, O_WRONLY)) < 0) {
        LOGE("Unable to open ums lunfile (%s)", strerror(errno));
        return;
    }

	if (write(fd2, vol2->device, strlen(vol2->device)) < 0) {
        LOGE("Unable to write to ums lunfile (%s)", strerror(errno));
        close(fd2);
        return;
    }

    static char* headers[] = {  "","USB Mass Storage device",
								"",
                                "Leaving this menu unmount sdcard",
                                "& internal storage from your PC",
                                NULL
    };

    static char* list[] = { "Unmount", NULL };
    for (;;)
    {
		gCurrentIcon = BACKGROUND_ICON_USB;
        int chosen_item = get_menu_selection(headers, list, 0, 0);
        if (chosen_item == GO_BACK || chosen_item == 0) {
            break;
		}
    }

    if ((fd = open(BOARD_UMS_LUNFILE, O_WRONLY)) < 0) {
        LOGE("Unable to open ums lunfile (%s)", strerror(errno));
        return;
    }

    char ch = 0;
    if (write(fd, &ch, 1) < 0) {
        LOGE("Unable to write to ums lunfile (%s)", strerror(errno));
        close(fd);
        return;
    }
	
	if ((fd2 = open(BOARD_UMS_LUNFILE2, O_WRONLY)) < 0) {
        LOGE("Unable to open ums lunfile (%s)", strerror(errno));
        return;
    }

    if (write(fd2, &ch, 1) < 0) {
        LOGE("Unable to write to ums lunfile (%s)", strerror(errno));
        close(fd2);
        return;
    }
}

int confirm_selection(const char* title, const char* confirm)
{
    struct stat info;
    if (0 == stat("/sdcard/clockworkmod/.no_confirm", &info))
        return 1;

    char* confirm_headers[]  = { "", title, "  THIS CAN NOT BE UNDONE.", NULL };
	if (0 == stat("/sdcard/clockworkmod/.one_confirm", &info)) {
		char* items[] = { "No",
						confirm, //" Yes -- wipe partition",   // [1]
						NULL };
		int chosen_item = get_menu_selection(confirm_headers, items, 0, 0);
		return chosen_item == 1;
	}
	else {
		char* items[] = { "No",
						"No",
						"No",
						"No",
						"No",
						"No",
						"No",
						confirm, //" Yes -- wipe partition",   // [7]
						"No",
						"No",
						"No",
						NULL };
		int chosen_item = get_menu_selection(confirm_headers, items, 0, 0);
		return chosen_item == 7;
	}
}


int format_device(const char *device, const char *path, const char *fs_type) {
    Volume* v = volume_for_path(path);
    if (v == NULL) {
        // no /sdcard? let's assume /data/media
        if (strstr(path, "/sdcard") == path && is_data_media()) {
            return format_unknown_device(NULL, path, NULL);
        }
        // silent failure for sd-ext
        if (strcmp(path, "/sd-ext") == 0)
            return -1;
        LOGE("unknown volume \"%s\"\n", path);
        return -1;
    }
    if (strstr(path, "/data") == path && volume_for_path("/sdcard") == NULL && is_data_media()) {
        return format_unknown_device(NULL, path, NULL);
    }
    if (strcmp(fs_type, "ramdisk") == 0) {
        // you can't format the ramdisk.
        LOGE("can't format_volume \"%s\"", path);
        return -1;
    }
/*
    if (strcmp(fs_type, "rfs") == 0) {
        if (ensure_path_unmounted(path) != 0) {
            LOGE("format_volume failed to unmount \"%s\"\n", v->mount_point);
            return -1;
        }
        if (0 != format_rfs_device(device, path)) {
            LOGE("format_volume: format_rfs_device failed on %s\n", device);
            return -1;
        }
        return 0;
    }
*/ 
    if (strcmp(v->mount_point, path) != 0) {
        return format_unknown_device(v->device, path, NULL);
    }

    if (ensure_path_unmounted(path) != 0) {
        LOGE("format_volume failed to unmount \"%s\"\n", v->mount_point);
        return -1;
    }
/*
    if (strcmp(fs_type, "yaffs2") == 0 || strcmp(fs_type, "mtd") == 0) {
        mtd_scan_partitions();
        const MtdPartition* partition = mtd_find_partition_by_name(device);
        if (partition == NULL) {
            LOGE("format_volume: no MTD partition \"%s\"\n", device);
            return -1;
        }

        MtdWriteContext *write = mtd_write_partition(partition);
        if (write == NULL) {
            LOGW("format_volume: can't open MTD \"%s\"\n", device);
            return -1;
        } else if (mtd_erase_blocks(write, -1) == (off_t) -1) {
            LOGW("format_volume: can't erase MTD \"%s\"\n", device);
            mtd_write_close(write);
            return -1;
        } else if (mtd_write_close(write)) {
            LOGW("format_volume: can't close MTD \"%s\"\n",device);
            return -1;
        }
        return 0;
    }

    if (strcmp(fs_type, "ext4") == 0) {
        int length = 0;
        if (strcmp(v->fs_type, "ext4") == 0) {
            // Our desired filesystem matches the one in fstab, respect v->length
            length = v->length;
        }
        reset_ext4fs_info();
        int result = make_ext4fs(device, length);
        if (result != 0) {
            LOGE("format_volume: make_extf4fs failed on %s\n", device);
            return -1;
        }
        return 0;
    }
*/
    return format_unknown_device(device, path, fs_type);
}

int format_unknown_device(const char *device, const char* path, const char *fs_type)
{
    LOGI("Formatting unknown device.fstype=%s\n",fs_type);
/*
    if (fs_type != NULL && get_flash_type(fs_type) != UNSUPPORTED)
        return erase_raw_partition(fs_type, device);

    // if this is SDEXT:, don't worry about it if it does not exist.
    if (0 == strcmp(path, "/sd-ext"))
    {
        struct stat st;
        Volume *vol = volume_for_path("/sd-ext");
        if (vol == NULL || 0 != stat(vol->device, &st))
        {
            ui_print("No app2sd partition found. Skipping format of /sd-ext.\n");
            return 0;
        }
    }

    if (NULL != fs_type) {
        if (strcmp("ext3", fs_type) == 0) {
            LOGI("Formatting ext3 device.\n");
            if (0 != ensure_path_unmounted(path)) {
                LOGE("Error while unmounting %s.\n", path);
                return -12;
            }
            return format_ext3_device(device);
        }

        if (strcmp("ext2", fs_type) == 0) {
            LOGI("Formatting ext2 device.\n");
            if (0 != ensure_path_unmounted(path)) {
                LOGE("Error while unmounting %s.\n", path);
                return -12;
            }
            return format_ext2_device(device);
        }
    }
*/
    if (0 != ensure_path_mounted(path))
    {
        ui_print("Error mounting %s!\n", path);
        ui_print("Skipping format...\n");
        return 0;
    }

    static char tmp[PATH_MAX];
	LOGI("Format mode : erase (rm -rf %s/*)\n", path);
	ui_print("Format mode : erase (rm -rf)...\n");
	ui_print("Remove immutable flag...\n");
	sprintf(tmp, "chattr -i -R %s/", path);
	__system(tmp);
	ui_print("Erasing %s...\n\n", basename(path));

    if (strcmp(path, "/data") == 0) {
        sprintf(tmp, "cd /data ; for f in $(ls -a | grep -v ^media$); do rm -rf $f; done");
        __system(tmp);
    }
    else {
        sprintf(tmp, "rm -rf %s/*", path);
        __system(tmp);
        sprintf(tmp, "rm -rf %s/.*", path);
        __system(tmp);
    }

	if (strcmp(path, "/cache") == 0) return 0;
	if (strcmp(path, "/preinstall") == 0) return 0;

   	ensure_path_unmounted(path);
    return 0;
}

//#define MOUNTABLE_COUNT 5
//#define DEVICE_COUNT 4
//#define MMC_COUNT 2

typedef struct {
    char mount[255];
    char unmount[255];
    Volume* v;
} MountMenuEntry;

typedef struct {
    char txt[255];
    Volume* v;
} FormatMenuEntry;

int is_safe_to_format(char* name)
{
    char str[255];
    char* partition;
    property_get("ro.cwm.forbid_format", str, "/misc,/recovery,/pds,/boot,/cid,/system,/cdrom,/preinstall");

    partition = strtok(str, ", ");
    while (partition != NULL) {
        if (strcmp(name, partition) == 0) {
			LOGI("%s is not safe to format\n", name);
            return 0;
        }
        partition = strtok(NULL, ", ");
    }

    return 1;
}

void show_partition_menu()
{
    static char* headers[] = {  "","Mounts and Storage Menu",
                                NULL
    };

    static MountMenuEntry* mount_menue = NULL;
    static FormatMenuEntry* format_menue = NULL;

    typedef char* string;

    int i, mountable_volumes, formatable_volumes;
    int num_volumes;
    Volume* device_volumes;

    num_volumes = get_num_volumes();
    device_volumes = get_device_volumes();

    string options[255];

    if(!device_volumes)
		    return;

		mountable_volumes = 0;
		formatable_volumes = 0;

		mount_menue = malloc(num_volumes * sizeof(MountMenuEntry));
		format_menue = malloc(num_volumes * sizeof(FormatMenuEntry));

		for (i = 0; i < num_volumes; ++i) {
  			Volume* v = &device_volumes[i];
  			if(strcmp("ramdisk", v->fs_type) != 0 && strcmp("mtd", v->fs_type) != 0 && strcmp("emmc", v->fs_type) != 0 && strcmp("bml", v->fs_type) != 0) {
    				sprintf(&mount_menue[mountable_volumes].mount, "mount %s", v->mount_point);
    				sprintf(&mount_menue[mountable_volumes].unmount, "unmount %s", v->mount_point);
    				mount_menue[mountable_volumes].v = &device_volumes[i];
    				++mountable_volumes;
    				if (is_safe_to_format(v->mount_point)) {
      					sprintf(&format_menue[formatable_volumes].txt, "format %s", v->mount_point);
      					format_menue[formatable_volumes].v = &device_volumes[i];
      					++formatable_volumes;
    				}
  		  }
  		  else if (strcmp("ramdisk", v->fs_type) != 0 && strcmp("mtd", v->fs_type) == 0 && is_safe_to_format(v->mount_point))
  		  {
    				sprintf(&format_menue[formatable_volumes].txt, "format %s", v->mount_point);
    				format_menue[formatable_volumes].v = &device_volumes[i];
    				++formatable_volumes;
  			}
		}


    static char* confirm_format  = "Confirm format?";
    static char* confirm = "Yes - Format";
    char confirm_string[255];

    for (;;)
    {
			gCurrentIcon = BACKGROUND_ICON_MOUNT;
    		for (i = 0; i < mountable_volumes; i++)
    		{
    			MountMenuEntry* e = &mount_menue[i];
    			Volume* v = e->v;
    			if(is_path_mounted(v->mount_point))
    				options[i] = e->unmount;
    			else
    				options[i] = e->mount;
    		}

    		for (i = 0; i < formatable_volumes; i++)
    		{
    			FormatMenuEntry* e = &format_menue[i];

    			options[mountable_volumes+i] = e->txt;
    		}

          options[mountable_volumes + formatable_volumes] = NULL;

        int chosen_item = get_menu_selection(headers, &options, 0, 0);
        if (chosen_item == GO_BACK)
            break;
        if (chosen_item < mountable_volumes) {
			      MountMenuEntry* e = &mount_menue[chosen_item];
            Volume* v = e->v;

            if (is_path_mounted(v->mount_point))
            {
                if (0 != ensure_path_unmounted(v->mount_point))
                    ui_print("Error unmounting %s!\n", v->mount_point);
            }
            else
            {
                if (0 != ensure_path_mounted(v->mount_point))
                    ui_print("Error mounting %s!\n",  v->mount_point);
            }
        }
        else if (chosen_item < (mountable_volumes + formatable_volumes))
        {
            chosen_item = chosen_item - mountable_volumes;
            FormatMenuEntry* e = &format_menue[chosen_item];
            Volume* v = e->v;

            sprintf(confirm_string, "%s - %s", v->mount_point, confirm_format);

            if (!confirm_selection(confirm_string, confirm))
                continue;
            ui_print("Formatting %s...\n", v->mount_point);
            if (0 != format_volume(v->mount_point))
                ui_print("Error formatting %s!\n", v->mount_point);
            else
                ui_print("Done.\n");
        }
    }

    free(mount_menue);
    free(format_menue);
}

void show_nandroid_advanced_backup_menu(const char* loc)
{
	char path[PATH_MAX];
    nandroid_generate_timestamp_path(path, loc);
    if (ensure_path_mounted(path) != 0) {
        LOGE ("Can't mount partition\n");
        return;
    }

    static char* headers[] = {  "","Nandroid Advanced Backup",
                                NULL
    };

    static char* list[] = { "Backup pds",
							"Backup system",
                            "Backup data",
                            "Backup cache",
                            "Backup webtop",
                            "Backup preinstall",
                            NULL
    };

    int chosen_item = get_menu_selection(headers, list, 0, 0);
    switch (chosen_item)
    {
        case 0:
            nandroid_adv_backup(path, 1, 0, 0, 0, 0, 0);
            break;
        case 1:
            nandroid_adv_backup(path, 0, 1, 0, 0, 0, 0);
            break;
        case 2:
            nandroid_adv_backup(path, 0, 0, 1, 0, 0, 0);
            break;
        case 3:
            nandroid_adv_backup(path, 0, 0, 0, 1, 0, 0);
            break;
        case 4:
            nandroid_adv_backup(path, 0, 0, 0, 0, 1, 0);
            break;
        case 5:
            nandroid_adv_backup(path, 0, 0, 0, 0, 0, 1);
            break;
    }
}

void show_nandroid_advanced_restore_menu(const char* path)
{
    if (ensure_path_mounted(path) != 0) {
        LOGE ("Can't mount %\n",path);
        return;
    }

    static char* advancedheaders[] = {  "","Choose an image to restore",
								"",
                                "Choose an image to restore",
                                "first. The next menu will",
                                "you more options.",
                                NULL
    };

    char tmp[PATH_MAX];
    sprintf(tmp, "%s/clockworkmod/backup/", path);
    char* file = choose_file_menu(tmp, NULL, advancedheaders);
    if (file == NULL)
        return;

    static char* headers[] = {  "","Nandroid Advanced Restore",
                                NULL
    };

    static char* list[] = { "Restore pds",
                            "Restore system",
                            "Restore data",
                            "Restore cache",
                            "Restore webtop",
                            "Restore preinstall",
                            NULL
    };

    static char* confirm_restore  = "Confirm restore?";

    int chosen_item = get_menu_selection(headers, list, 0, 0);
    switch (chosen_item)
    {
        case 0:
            if (confirm_selection(confirm_restore, "Yes - Restore pds"))
                nandroid_restore(file, 1, 0, 0, 0, 0, 0);
            break;
        case 1:
            if (confirm_selection(confirm_restore, "Yes - Restore system"))
                nandroid_restore(file, 0, 1, 0, 0, 0, 0);
            break;
        case 2:
            if (confirm_selection(confirm_restore, "Yes - Restore data"))
                nandroid_restore(file, 0, 0, 1, 0, 0, 0);
            break;
        case 3:
            if (confirm_selection(confirm_restore, "Yes - Restore cache"))
                nandroid_restore(file, 0, 0, 0, 1, 0, 0);
            break;
        case 4:
            if (confirm_selection(confirm_restore, "Yes - Restore webtop"))
                nandroid_restore(file, 0, 0, 0, 0, 1, 0);
            break;
        case 5:
            if (confirm_selection(confirm_restore, "Yes - Restore preinstall"))
                nandroid_restore(file, 0, 0, 0, 0, 0, 1);
            break;
    }
}

void show_path_chooser(int zip, const char* location, const char* title) {
	static char* sel[] = {"","Please select a location",NULL};
	static char* plist[] = { "Micro SD","Internal storage","USB OTG 1","USB OTG 2","USB OTG 3","USB OTG 4",NULL};
    int chosen_item = get_menu_selection(sel, plist, 0, 0);
    switch (chosen_item) {
		case 0:
			sprintf(location, "%s", "/sdcard");
			break;
		case 1:
			sprintf(location, "%s", "/emmc");
			break;
		case 2:
		case 3:
		case 4:
		case 5:
			sprintf(location, "/otg/usb%d", chosen_item);
			break;
		default:
			break;
	}
	if (ensure_path_mounted(location) != 0) {
		sprintf(location, "%s", def_location);
		sprintf(title,"File location : %s",plist[1]);		
	    ui_print("Fallback to %s\n", plist[1]);
    } else {
		if ((chosen_item >= 0) && (chosen_item <= 5)) {
			sprintf(title,"File location : %s",plist[chosen_item]);
			if (zip == 1) sprintf(location + strlen(location), "/");
		}
	}
}

void get_title(int zip, const char* location, const char* title) {
	sprintf(location, "%s", def_location);
	if (zip == 1) sprintf(location + strlen(location), "/");
	if (!strcmp(def_location, "/emmc")) {
		sprintf(title, "File location : Internal storage");
	} else {
		sprintf(title, "File location : Micro SD");
	}
}

void show_nandroid_menu()
{
    static char* list[] = { "Fast backup",
                            "Full backup",
                            "Selective backup",
                            "Restore",
                            "Selective restore",
                            "Change backup location",
                            NULL
    };
	static char* titles[] = {"","Nandroid backup & restore", "", "internal storage",NULL};
	static char* confirm  = "Backup these partition.Confirm?";
    
	char location[PATH_MAX] = "";
	char title[PATH_MAX] = "";	
	get_title(0,location,title);

    for (;;) {
		titles[3] = title;
		gCurrentIcon = BACKGROUND_ICON_BACKUP;
		int chosen_item = get_menu_selection(titles, list, 0, 0);
    	switch (chosen_item) {
       		case 0:
				{
        	    	if (confirm_selection(confirm, "Yes - Backup data, cache & system")) {
							char backup_path[PATH_MAX];
						    nandroid_generate_timestamp_path(backup_path,location);
							nandroid_backup(backup_path);
					}
        	    	break;
				}
       		case 1:
				{
        	    	if (confirm_selection(confirm, "Yes - Backup all")) {
							char backup_path[PATH_MAX];
						    nandroid_generate_timestamp_path(backup_path,location);
							nandroid_backups(backup_path);
					}
        	    	break;
				}
        	case 2:
        	    show_nandroid_advanced_backup_menu(location);
        	    break;
        	case 3:
        	    show_nandroid_restore_menu(location);
        	    break;
			case 4:
				show_nandroid_advanced_restore_menu(location);
				break;
        	case 5:
				show_path_chooser(0,location,title);
        	    break;
            default:
				return;
    	}
	}
}

void wipe_battery_stats()
{
    ensure_path_mounted("/data");
    remove("/data/system/batterystats.bin");
    ensure_path_unmounted("/data");
    ui_print("Battery Stats wiped.\n");
}

void write_sys(const char* file, int value) {
	FILE* f = fopen(file, "w");
	if (f != NULL) {
    	fprintf(f, "%d", value);
    	fclose(f);
  	}
}

int read_sys(const char* file) {
	int i = 0;
	FILE* f = fopen(file, "r");
	if (f != NULL) {
		fscanf(f,"%d", &i); 
    	fclose(f);
  	}
	return i;
}

void set_led(const char* color, int value) {
	char led_path[PATH_MAX];
	sprintf(led_path, "/sys/class/leds/%s/brightness", color);
	write_sys(led_path, value);
}

int get_led(const char* color) {
	char led_path[PATH_MAX];
	sprintf(led_path, "/sys/class/leds/%s/brightness", color);
	return read_sys(led_path);
}

void set_amoled(int value) { write_sys("/sys/class/backlight/430_540_960_amoled_bl/brightness", value); }
int get_amoled() { return read_sys("/sys/class/backlight/430_540_960_amoled_bl/brightness"); }

void show_adjust_brightness_menu()
{
	static char* items[] = { "Brightness up", "Brightness down", NULL };
    static char* headers[] = {  "","Adjust Brightness", NULL };
    for (;;)
    {
		int old_val = get_amoled();
		ui_print("Brightness : %d%%\n", (old_val*100)/255);
        int chosen_item = get_menu_selection(headers, items, 0, 0);
        switch (chosen_item)
        {
            case 0:
				old_val += 10;
				if (old_val > 255) old_val = 255;
				set_amoled(old_val);
                break;
            case 1:
				old_val -= 10;
				if (old_val < 10) old_val = 10;
				set_amoled(old_val);
                break;
            default:
				return;
        }
    }
}
void show_advanced_menu()
{
    static char* headers[] = {  "","Advanced and Debugging Menu",
                                NULL
    };

    static char* list[] = { "Restart ADB daemon",
                            "Connect as USB mass storage",
                            "Adjust screen brightness",
                            "Turn on/off keypad light",
                            "Enable touch & slide to select",
                            "Report error",
                            "Key test",
                            "Show log",
                            "Partition SD card",
                            "Fix Permissions",
#ifdef BOARD_HAS_SDCARD_INTERNAL
                            "Partition Internal SD Card",
#endif
                            NULL
    };

    for (;;)
    {
		gCurrentIcon = BACKGROUND_ICON_ADVANCE;
        int chosen_item = get_menu_selection(headers, list, 0, 0);
        if (chosen_item == GO_BACK)
            break;
        switch (chosen_item)
        {
            case 0:
				property_set("sys.usb.config", "mass_storage,adb");
                break;
            case 1:
				show_mount_usb_storage_menu();
                break;
            case 2:
  				show_adjust_brightness_menu();
                break;
            case 3:
				set_led("button-backlight",!get_led("button-backlight"));
                break;
			case 4:
				touch_select = !touch_select;
                break;
            case 5:
                handle_failure(1);
                break;
            case 6:
            {
                ui_print("Outputting key codes.\n");
                ui_print("Go back to end debugging.\n");
                int key;
                int action;
                do
                {
                    key = ui_wait_key();
                    action = device_handle_key(key, 1);
                    ui_print("Key: %d\n", key);
                }
                while (action != GO_BACK);
                break;
            }
            case 7:
            {
                ui_printlogtail(20);
                break;
            }
            case 8:
            {
                static char* ext_sizes[] = { "128M",
                                             "256M",
                                             "512M",
                                             "1024M",
                                             "2048M",
                                             "4096M",
                                             NULL };

                static char* swap_sizes[] = { "0M",
                                              "32M",
                                              "64M",
                                              "128M",
                                              "256M",
                                              NULL };

                static char* ext_headers[] = { "","Ext Size", NULL };
                static char* swap_headers[] = { "","Swap Size", NULL };

                int ext_size = get_menu_selection(ext_headers, ext_sizes, 0, 0);
                if (ext_size == GO_BACK)
                    continue;

                int swap_size = get_menu_selection(swap_headers, swap_sizes, 0, 0);
                if (swap_size == GO_BACK)
                    continue;

                char sddevice[256];
                Volume *vol = volume_for_path("/sdcard");
                strcpy(sddevice, vol->device);
                // we only want the mmcblk, not the partition
                sddevice[strlen("/dev/block/mmcblkX")] = NULL;
                char cmd[PATH_MAX];
                setenv("SDPATH", sddevice, 1);
                sprintf(cmd, "sdparted -es %s -ss %s -efs ext3 -s", ext_sizes[ext_size], swap_sizes[swap_size]);
                ui_print("Partitioning SD Card... please wait...\n");
                if (0 == __system(cmd))
                    ui_print("Done!\n");
                else
                    ui_print("An error occured while partitioning your SD Card. Please see /tmp/recovery.log for more details.\n");
                break;
            }
            case 9:
            {
                ensure_path_mounted("/system");
                ensure_path_mounted("/data");
                ui_print("Fixing permissions...\n");
                __system("fix_permissions");
                ui_print("Done!\n");
                break;
            }
            case 10:
            {
                static char* ext_sizes[] = { "128M",
                                             "256M",
                                             "512M",
                                             "1024M",
                                             "2048M",
                                             "4096M",
                                             NULL };

                static char* swap_sizes[] = { "0M",
                                              "32M",
                                              "64M",
                                              "128M",
                                              "256M",
                                              NULL };

                static char* ext_headers[] = { "Data Size", "", NULL };
                static char* swap_headers[] = { "Swap Size", "", NULL };

                int ext_size = get_menu_selection(ext_headers, ext_sizes, 0, 0);
                if (ext_size == GO_BACK)
                    continue;

                int swap_size = 0;
                if (swap_size == GO_BACK)
                    continue;

                char sddevice[256];
                Volume *vol = volume_for_path("/emmc");
                strcpy(sddevice, vol->device);
                // we only want the mmcblk, not the partition
                sddevice[strlen("/dev/block/mmcblkX")] = NULL;
                char cmd[PATH_MAX];
                setenv("SDPATH", sddevice, 1);
                sprintf(cmd, "sdparted -es %s -ss %s -efs ext3 -s", ext_sizes[ext_size], swap_sizes[swap_size]);
                ui_print("Partitioning Internal SD Card... please wait...\n");
                if (0 == __system(cmd))
                    ui_print("Done!\n");
                else
                    ui_print("An error occured while partitioning your Internal SD Card. Please see /tmp/recovery.log for more details.\n");
                break;
            }
        }
    }
}

void write_fstab_root(char *path, FILE *file)
{
    Volume *vol = volume_for_path(path);
    if (vol == NULL) {
        LOGW("Unable to get recovery.fstab info for %s during fstab generation!\n", path);
        return;
    }

    char device[200];
    strcpy(device, vol->device);

    fprintf(file, "%s ", device);
    fprintf(file, "%s ", path);
    // special case rfs cause auto will mount it as vfat on samsung.
    fprintf(file, "%s rw\n", vol->fs_type2 != NULL && strcmp(vol->fs_type, "rfs") != 0 ? "auto" : vol->fs_type);
}

void create_fstab()
{
    struct stat info;
    __system("touch /etc/mtab");
    FILE *file = fopen("/etc/fstab", "w");
    if (file == NULL) {
        LOGW("Unable to create /etc/fstab!\n");
        return;
    }
    write_fstab_root("/cache", file);
    write_fstab_root("/data", file);
    write_fstab_root("/pds", file);
    write_fstab_root("/emmc", file);
    write_fstab_root("/system", file);
    write_fstab_root("/sdcard", file);
    write_fstab_root("/webtop", file);
	write_fstab_root("/preinstall", file);
    write_fstab_root("/otg/usb1", file);
    write_fstab_root("/otg/usb2", file);
    write_fstab_root("/otg/usb3", file);
    write_fstab_root("/otg/usb4", file);
    fclose(file);
    LOGI("Completed outputting fstab.\n");
}

/*
int bml_check_volume(const char *path) {
    ui_print("Checking %s...\n", path);
    ensure_path_unmounted(path);
    if (0 == ensure_path_mounted(path)) {
        ensure_path_unmounted(path);
        return 0;
    }
    
    Volume *vol = volume_for_path(path);
    if (vol == NULL) {
        LOGE("Unable process volume! Skipping...\n");
        return 0;
    }
    
    ui_print("%s may be rfs. Checking...\n", path);
    char tmp[PATH_MAX];
    sprintf(tmp, "mount -t rfs %s %s", vol->device, path);
    int ret = __system(tmp);
    printf("%d\n", ret);
    return ret == 0 ? 1 : 0;
}
*/

void process_volumes() {
    create_fstab();

    if (is_data_media()) {
        setup_data_media();
    }

    return;
/*
    // dead code.
    if (device_flash_type() != BML)
        return;

    ui_print("Checking for ext4 partitions...\n");
    int ret = 0;
    ret = bml_check_volume("/system");
    ret |= bml_check_volume("/data");
    if (has_datadata())
        ret |= bml_check_volume("/datadata");
    ret |= bml_check_volume("/cache");
    
    if (ret == 0) {
        ui_print("Done!\n");
        return;
    }
    
    char backup_path[PATH_MAX];
    time_t t = time(NULL);
    char backup_name[PATH_MAX];
    struct timeval tp;
    gettimeofday(&tp, NULL);
    sprintf(backup_name, "before-ext4-convert-%d", tp.tv_sec);
    sprintf(backup_path, "/sdcard/clockworkmod/backup/%s", backup_name);

    ui_set_show_text(1);
    ui_print("Filesystems need to be converted to ext4.\n");
    ui_print("A backup and restore will now take place.\n");
    ui_print("If anything goes wrong, your backup will be\n");
    ui_print("named %s. Try restoring it\n", backup_name);
    ui_print("in case of error.\n");

    nandroid_backup(backup_path);
    nandroid_restore(backup_path, 1, 1, 1, 1, 1, 0);
    ui_set_show_text(0);*/
}

void handle_failure(int ret)
{
    if (ret == 0)
        return;
    if (0 != ensure_path_mounted("/emmc"))
        return;
    mkdir("/sdcard/clockworkmod", S_IRWXU);
    __system("cp /tmp/recovery.log /emmc/clockworkmod/recovery.log");
    ui_print("/tmp/recovery.log was copied to /emmc/clockworkmod/recovery.log.\n");
}

int is_path_mounted(const char* path) {
    Volume* v = volume_for_path(path);
    if (v == NULL) {
        return 0;
    }
    if (strcmp(v->fs_type, "ramdisk") == 0) {
        // the ramdisk is always mounted.
        return 1;
    }

    int result;
    result = scan_mounted_volumes();
    if (result < 0) {
        LOGE("failed to scan mounted volumes\n");
        return 0;
    }

    const MountedVolume* mv =
        find_mounted_volume_by_mount_point(v->mount_point);
    if (mv) {
        // volume is already mounted
        return 1;
    }
    return 0;
}

int has_datadata() {
    Volume *vol = volume_for_path("/datadata");
    return vol != NULL;
}

int volume_main(int argc, char **argv) {
    load_volume_table();
    return 0;
}
