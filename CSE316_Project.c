#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_SIZE 100
#define MAX_FILE_NAME 20
#define DISK_SIZE 1000

typedef struct
{
    char name[MAX_FILE_NAME];
    int start_block;
    int size;
} file;

int allocate_contiguous_blocks(int size, int *disk)
{
    int start_block = -1;
    int consecutive_blocks = 0;

    for (int i = 0; i < DISK_SIZE; i++)
    {
        if (disk[i] == 0)
        {
            if (consecutive_blocks == 0)
            {
                start_block = i;
            }
            consecutive_blocks++;
            if (consecutive_blocks == size)
            {
                for (int j = start_block; j < start_block + size; j++)
                {
                    disk[j] = 1;
                }
                return start_block;
            }
        }
        else
        {
            consecutive_blocks = 0;
        }
    }
    return -1;
}

void delete_file(char *name, file *files, int *file_count, int *disk)
{
    for (int i = 0; i < *file_count; i++)
    {
        if (strcmp(files[i].name, name) == 0)
        {
            for (int j = files[i].start_block; j < files[i].start_block + files[i].size; j++)
            {
                disk[j] = 0;
            }
            for (int j = i; j < *file_count - 1; j++)
            {
                files[j] = files[j + 1];
            }
            (*file_count)--;
            printf("%s deleted successfully!\n", name);
            return;
        }
    }
    printf("File %s not found\n", name);
}

void rename_file(char *old_name, char *new_name, file *files, int file_count)
{
    for (int i = 0; i < file_count; i++)
    {
        if (strcmp(files[i].name, old_name) == 0)
        {
            strcpy(files[i].name, new_name);
            printf("%s renamed to %s\n", old_name, new_name);
            return;
        }
    }
    printf("File %s not found\n", old_name);
}

void move_file(char *name, int new_block, file *files, int file_count, int *disk)
{
    for (int i = 0; i < file_count; i++)
    {
        if (strcmp(files[i].name, name) == 0)
        {
            int old_block = files[i].start_block;
            int size = files[i].size;
            if (new_block + size > DISK_SIZE)
            {
                printf("Error: Not enough space to move file\n");
                return;
            }
            for (int j = old_block; j < old_block + size; j++)
            {
                disk[j] = 0;
            }
            files[i].start_block = new_block;
            for (int j = new_block; j < new_block + size; j++)
            {
                disk[j] = 1;
            }
            printf("%s moved from block %d to block %d\n", name, old_block, new_block);
            return;
        }
    }
    printf("File %s not found\n", name);
}

// Function to calculate and print fragmentation and wasted blocks
void calculateAndPrintFragmentation(int *disk)
{
    int consecutive_free_blocks = 0;
    int fragment_count = 0;
    int wasted_blocks = 0;
    int current_block_status = disk[0]; // Initialize with the status of the first block

    for (int i = 0; i < DISK_SIZE; i++)
    {
        if (disk[i] == current_block_status)
        {
            consecutive_free_blocks++;
        }
        else
        {
            if (consecutive_free_blocks > 0)
            {
                fragment_count++;
                wasted_blocks += consecutive_free_blocks;
                consecutive_free_blocks = 0;
            }
            current_block_status = disk[i];
        }
    }

    // Check for consecutive free blocks at the end
    if (consecutive_free_blocks > 0)
    {
        fragment_count++;
        wasted_blocks += consecutive_free_blocks;
    }

    float avg_fragment_size = (fragment_count > 0) ? (float)wasted_blocks / fragment_count : 0;

    printf("Fragmentation: %d\n", fragment_count);
    printf("Average Fragment Size: %f\n", avg_fragment_size);
    printf("Wasted blocks: %d\n", wasted_blocks);
}

void printDiskStatus(int time, int *disk)
{
    printf("Disk status:\n");
    printf("Disk state at end of time %d:\n", time);

    for (int i = 0; i < DISK_SIZE; i++)
    {
        printf("%d ", disk[i]);
    }

    printf("\n");
}

int main()
{
    int disk[DISK_SIZE] = {0};
    file files[MAX_FILE_SIZE];
    int file_count = 0;

    int time = 0;
    char action;

    while (1)
    {
        printf("\nTime %d\n", time);
        printf("Options: (C)reate, (D)elete, (R)ename, (M)ove, (Q)uit\n");
        scanf(" %c", &action);

        switch (action)
        {
        case 'C':
        case 'c':
        {
            if (file_count >= MAX_FILE_SIZE)
            {
                printf("Error: Maximum number of files reached\n");
            }
            else
            {
                char new_file_name[MAX_FILE_NAME];
                int new_file_size;
                printf("Enter name and size of the new file: ");
                scanf("%s %d", new_file_name, &new_file_size);
                if (new_file_size > DISK_SIZE)
                {
                    printf("Error: Not enough space to create file\n");
                }
                else
                {
                    int start_block = allocate_contiguous_blocks(new_file_size, disk);
                    if (start_block == -1)
                    {
                        printf("Error: Not enough space to create file\n");
                    }
                    else
                    {
                        file new_file;
                        strcpy(new_file.name, new_file_name);
                        new_file.start_block = start_block;
                        new_file.size = new_file_size;
                        files[file_count] = new_file;
                        file_count++;
                        printf("%s created successfully with size %d\n", new_file_name, new_file_size);
                    }
                }
            }
            calculateAndPrintFragmentation(disk);
            printDiskStatus(time, disk);
            break;
        }

        case 'D':
        case 'd':
        {
            char delete_file_name[MAX_FILE_NAME];
            printf("Enter the name of the file to delete: ");
            scanf("%s", delete_file_name);
            delete_file(delete_file_name, files, &file_count, disk);
            calculateAndPrintFragmentation(disk);
            printDiskStatus(time, disk);
            break;
        }

        case 'R':
        case 'r':
        {
            char old_file_name[MAX_FILE_NAME];
            char new_file_name2[MAX_FILE_NAME];
            printf("Enter the name of the file to rename: ");
            scanf("%s", old_file_name);
            printf("Enter the new name of the file: ");
            scanf("%s", new_file_name2);
            rename_file(old_file_name, new_file_name2, files, file_count);
            calculateAndPrintFragmentation(disk);
            printDiskStatus(time, disk);
            break;
        }

        case 'M':
        case 'm':
        {
            char move_file_name[MAX_FILE_NAME];
            int new_block;
            printf("Enter the name of the file to move: ");
            scanf("%s", move_file_name);
            printf("Enter the new block of the file: ");
            scanf("%d", &new_block);
            move_file(move_file_name, new_block, files, file_count, disk);
            calculateAndPrintFragmentation(disk);
            printDiskStatus(time, disk);
            break;
        }

        case 'Q':
        case 'q':
        {
            printf("Exiting the program.\n");
            exit(0);
        }

        default:
            printf("Invalid option. Please try again.\n");
        }

        time++;
    }

    return 0;
}

