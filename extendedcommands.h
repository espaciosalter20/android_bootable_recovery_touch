int show_power_options_menu();
void show_safety_options_menu();
void show_nandroid_advanced_backup_menu(const char* path);
void show_help_menu();
void show_path_chooser(int zip, const char* location, const char* title);
void get_title(int zip, const char* location, const char* title);
//razr led stuff
void write_sys(const char* file, int value);
int read_sys(const char* file);
void set_led(const char* color, int value);
int get_led(const char* color);
void set_amoled(int value);
int get_amoled();

extern int signature_check_enabled;
extern int script_assert_enabled;

void
toggle_signature_check();

void
toggle_script_asserts();

void
show_choose_zip_menu();

int
do_nandroid_backup(const char* backup_name);

int
do_nandroid_restore();

void
show_nandroid_restore_menu(const char* path);

void
show_nandroid_advanced_restore_menu(const char* path);

void
show_nandroid_menu();

void
show_partition_menu();

void
show_choose_zip_menu();

int
install_zip(const char* packagefilepath);

int
__system(const char *command);

void
show_advanced_menu();

int format_unknown_device(const char *device, const char* path, const char *fs_type);

void
wipe_battery_stats();

void create_fstab();

int has_datadata();

void handle_failure(int ret);

void process_volumes();

int extendedcommand_file_exists();

void show_install_update_menu();

int confirm_selection(const char* title, const char* confirm);

int run_and_remove_extendedcommand();
