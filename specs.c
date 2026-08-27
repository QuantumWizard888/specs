/*
 * specs.c
 * Displays system and hardware information
 */
#include <linux/limits.h>
#include <linux/sysinfo.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <sys/statvfs.h>

int main()
{
    struct utsname utsname_data;
    struct sysinfo sysinfo_data;
    struct statvfs statvfs_data;

    if (uname(&utsname_data) < 0) {
        perror("Error: uname");

        return 1;
    }

    if (sysinfo(&sysinfo_data) < 0){
        perror("Error: sysinfo");

        return 1;
    }

    if (statvfs("/", &statvfs_data) < 0) {
        perror("Error: statvfs");

        return 1;
    }

    // RAM calculations
    unsigned long long total_ram_mb = ((unsigned long long)sysinfo_data.totalram * sysinfo_data.mem_unit)/1024/1024;
    unsigned long long free_ram_mb = ((unsigned long long)sysinfo_data.freeram * sysinfo_data.mem_unit)/1024/1024;
    unsigned long long used_ram_mb = total_ram_mb - free_ram_mb;
    double total_ram_gb = ((double)sysinfo_data.totalram * sysinfo_data.mem_unit)/1024.0/1024/1024;
    double free_ram_gb = ((double)sysinfo_data.freeram * sysinfo_data.mem_unit)/1024.0/1024/1024;
    double used_ram_gb = ((double)used_ram_mb)/1024.0;

    // Disk space calculations
    unsigned long long fs_block_size = statvfs_data.f_frsize;
    unsigned long long fs_total_space = statvfs_data.f_blocks * fs_block_size;
    unsigned long long fs_free_space = statvfs_data.f_bavail * fs_block_size;
    unsigned long long fs_used_space = fs_total_space - fs_free_space;

    double fs_total_space_gb = (double)fs_total_space/1024.0/1024/1024;
    double fs_free_space_gb = (double)fs_free_space/1024.0/1024/1024;
    double fs_used_space_gb = (double)fs_used_space/1024.0/1024/1024;

    printf("=== Kernel and OS info ===\n");
    printf("Kernel name: %s\n", utsname_data.sysname);
    printf("Kernel version: %s\n", utsname_data.release);
    printf("OS build version: %s\n", utsname_data.version);
    printf("Machine arch: %s\n", utsname_data.machine);
    printf("Machine name: %s\n", utsname_data.nodename);
    
    printf("=== Session info ===\n");
    printf("Uptime (seconds): %ld\n", sysinfo_data.uptime);
    printf("Uptime: %ld days and %02ld:%02ld:%02ld\n", sysinfo_data.uptime/86400, (sysinfo_data.uptime/3600) % 24, (sysinfo_data.uptime/60) % 60, sysinfo_data.uptime % 60);

    printf("=== RAM ===\n");
    printf("Total RAM:  %llu MB, %.3f GB\n", total_ram_mb, total_ram_gb);
    printf("Free RAM:   %llu MB, %.3f GB\n", free_ram_mb, free_ram_gb);
    printf("Used RAM:   %llu MB, %.3f GB\n", used_ram_mb, used_ram_gb);

    printf("=== Disk space ===\n");
    printf("Total:  %.3f GB\n", fs_total_space_gb);
    printf("Free:   %.3f GB\n", fs_free_space_gb);
    printf("Used:   %.3f GB\n", fs_used_space_gb);

    printf("=== CPU ===\n");
    char fbuffer[1024];
    FILE *fp = fopen("/proc/cpuinfo", "r");

    if (!fp) {
        perror("Error while reading /proc/cpuinfo");

        return 1;
    }

    while (fgets(fbuffer, sizeof(fbuffer), fp) != NULL) {
        if (strncmp(fbuffer, "processor\t: 1", 13) == 0) {
            break;
        }
        
        if (strstr(fbuffer, "vendor_id")  ||
            strstr(fbuffer, "model name") ||
            strstr(fbuffer, "cpu MHz")    ||
            strstr(fbuffer, "cache size") ||
            strstr(fbuffer, "cpu cores")  ||
            strstr(fbuffer, "flags")) {
            fputs(fbuffer, stdout);
        }
    }

    fclose(fp);
    
    printf("=== PCI devices ===\n");
    char pbuffer[1024];

    FILE *pfp = popen("lspci", "r");

    if (!pfp) {
        perror("Error while reading lspci command");

        return 1;
    }

    while (fgets(pbuffer, sizeof(pbuffer), pfp) != NULL) {
        fputs(pbuffer, stdout);
    }

    int pfp_status = pclose(pfp);

    if (pfp_status == -1) {
        perror("Error closing pipe for lspci");

        return 1;
    } else if (pfp_status != 0) {

        return 1;
    }

    return 0;
}