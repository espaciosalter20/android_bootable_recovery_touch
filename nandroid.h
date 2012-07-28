#ifndef NANDROID_H
#define NANDROID_H

int nandroid_main(int argc, char** argv);
int nandroid_backups(const char* loc);
int nandroid_backup(const char* backup_path);
int nandroid_adv_backup(const char* backup_path, int backup_pds, int backup_system, int backup_data, int backup_cache, int backup_webtop, int backup_preinstall);
int nandroid_restore(const char* backup_path, int restore_pds, int restore_system, int restore_data, int restore_cache, int restore_webtop, int restore_preinstall);
void nandroid_generate_timestamp_path(const char* backup_path, const char* loc);
#endif
