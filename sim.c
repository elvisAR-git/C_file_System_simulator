#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

// List of usable commands to reference
char *cmd[] = {"mkdir", "rmdir", "ls", "cd", "pwd", "create", "rm", "reload", "save", "menu", "quit", NULL};
char *cwd[100];
char *paths[200];
int lastPointer = 0;
void pwd();

int tokenize(char *pathname);
int len = 0;
struct entity
{
    char basename[64];
    char pathname[64];
    char parentFolder[64];
    char *nodeType[10];
};

struct entity fs[5000];

void innitialize()
{
    strcpy(fs[0].basename, "/");
    strcpy(fs[0].pathname, "/");
    strcpy(fs[0].parentFolder, "/");
    strcpy(fs[0].nodeType, "R");
    len++;
    strcpy(cwd, fs[0].pathname);
}

void mkdirf(char *pathname)
{
    char *newpathname[200];
    char **ptr = pathname;

    char *basename[100];
    char *parentFolder[100];
    char *nodeType[10];
    int x;
    bool first = true;
    int f;
    f = lastPointer;

    if (f == 0)
    {
        strcpy(newpathname, "/");
        strcpy(basename, ptr[0]);
        strcpy(parentFolder, "/");
        strcpy(nodeType, "D");
    }

    for (x = 0; x < f; x++)
    {
        if (ptr[x] != NULL)
        {
            if (first)
            {
                strcpy(newpathname, "/");
                strcat(newpathname, ptr[x]);
                first = false;
            }
            else
            {

                strcat(newpathname, "/");
                strcat(newpathname, ptr[x]);
            }
            if (x < f)
            {
                strcpy(parentFolder, newpathname);
            }
        }
        else
        {
            break;
        }
        strcpy(basename, ptr[x + 1]);

        strcpy(nodeType, "D");
    }
    int i;
    bool pass = true;
    bool path = false;
    for (i = 0; i < len; i++)
    {
        if (strcmp(fs[i].basename, basename) == 0 && strcmp(fs[i].parentFolder, parentFolder) == 0)
        {
            printf("\e[35mError, Folder already exists\n");
            pass = false;
            break;
        }

        if (strcmp(fs[i].basename, basename) == 0 && strcmp(fs[i].nodeType, "F") == 0)
        {
            pass = false;
            printf("\e[35m%s\n is a File", basename);
            break;
        }
        if (strcmp(fs[i].pathname, newpathname) == 0)
        {
            path = true;
        }
    }

    if (pass && path)
    {
        struct entity p;
        strcpy(p.pathname, newpathname);
        strcpy(p.nodeType, nodeType);
        strcpy(p.basename, basename);
        strcpy(p.parentFolder, parentFolder);

        addToFs(p);
    }
    lastPointer = 0;
    strcpy(newpathname, "");
    strcpy(basename, "");
    strcpy(parentFolder, "");
    strcpy(nodeType, "");
}

// the rmdir function
void rmdirf(char pathname[20])
{
    //code here
    char *newpathname[200];
    char **ptr = paths;

    char *basename[100];
    char *parentFolder[100];
    char *nodeType[10];
    int x;
    bool first = true;
    int f;
    f = lastPointer;
    char *temp[100];

    if (f == 0)

    {
        strcpy(temp, "/");
        strcat(temp, ptr[0]);
        strcpy(newpathname, temp);
        strcpy(basename, ptr[0]);
        strcpy(parentFolder, "/");
        strcpy(nodeType, "D");
    }
    else
    {
        for (x = 0; x < f; x++)
        {
            if (ptr[x] != NULL)
            {
                if (first)
                {
                    strcpy(newpathname, "/");
                    strcat(newpathname, ptr[x]);
                    first = false;
                }
                else
                {

                    strcat(newpathname, "/");
                    strcat(newpathname, ptr[x]);
                }
                if (x < f)
                {
                    strcpy(parentFolder, newpathname);
                }
            }
            else
            {
                break;
            }
            strcpy(basename, ptr[x + 1]);
        }

        strcat(newpathname, "/");
        strcat(newpathname, ptr[f]);
    }

    // printf("path to delete: %s\n", newpathname);
    // printf("basename to delete: %s\n", basename);

    int i;
    for (i = 0; i < len; i++)
    {
        if (strcmp(fs[i].basename, basename) == 0 && strcmp(fs[i].parentFolder, parentFolder) == 0)
        {
            if (strcmp(fs[i].nodeType, "F") == 0)
            {
                printf("\e[35m%s is a File\n", basename);
                return;
            }
            else
            {
                int d;
                for (d = 0; d < len; d++)
                {
                    if (strcmp(fs[d].parentFolder, fs[i].pathname) == 0)
                    {
                        printf("\e[35m%s Contains contents\n", newpathname);
                        return;
                    }
                }
                // delete element entity from file system
                int pos = i + 1;
                int size = len + 1;
                int j;

                for (j = pos - 1; j < size - 1; j++)
                {
                    fs[j] = fs[j + 1];
                }
                len--;
                return;
            }
        }
    }
}

void cd(char *pathname)
{
    //code here
    int i;
    for (i = 0; i < len; i++)
    {
        if (strcmp(fs[i].pathname, pathname) == 0)
        {
            // pathname exists
            if (strcmp(fs[i].nodeType, "D") == 0 || strcmp(fs[i].nodeType, "R") == 0)
            {
                // is a folder or root
                strcpy(cwd, pathname);
                return;
            }
        }
    }
    char *tempath[100];
    if (strcmp(cwd, "/") == 0)
    {

        strcpy(tempath, cwd);
        strcat(tempath, pathname);
    }
    else
    {
        strcpy(tempath, cwd);
        strcat(tempath, "/");
        strcat(tempath, pathname);
    }
    for (i = 0; i < len; i++)
    {
        if (strcmp(fs[i].pathname, tempath) == 0)
        {
            // pathname exists
            if (strcmp(fs[i].nodeType, "D") == 0 || strcmp(fs[i].nodeType, "R") == 0)
            {
                // is a folder or root
                strcpy(cwd, tempath);
                return;
            }
        }
    }
    printf("\e[35mCannot CD into %s\n", pathname);
}

void ls(char pathname[20])
{
    int i;
    //code here
    printf("\e[32m\n\n-------------------listing[\e[30m%s\e[32m]----------------------------------\n", cwd);
    for (i = 0; i < len; i++)
    {
        if (strcmp(fs[i].parentFolder, cwd) == 0)
        {
            printf("\e[32m  %s                    \e[35m%s\n", fs[i].pathname, fs[i].nodeType);
            // printf("\e[32m%s********", fs[i].nodeType);
            // printf("\e[32m%s\n", fs[i].basename);
        }
    }
    printf("\e[32m------------------------------------------------------------\n\n");
}

void pwd()
{
    printf("\e[0m%s\n", cwd);
}

void create(char *pathname)
{
    //code here
    char *newpathname[200];
    char **ptr = pathname;

    char *basename[100];
    char *parentFolder[100];
    char *nodeType[10];
    int x;
    bool first = true;
    int f;
    f = lastPointer;

    if (f == 0)
    {
        strcpy(newpathname, "/");
        strcpy(basename, ptr[0]);
        strcpy(parentFolder, "/");
        strcpy(nodeType, "F");
    }

    for (x = 0; x < f; x++)
    {
        if (ptr[x] != NULL)
        {
            if (first)
            {
                strcpy(newpathname, "/");
                strcat(newpathname, ptr[x]);
                first = false;
            }
            else
            {

                strcat(newpathname, "/");
                strcat(newpathname, ptr[x]);
            }

            if (x < f)
            {
                strcpy(parentFolder, newpathname);
            }
        }
        else
        {
            break;
        }
        strcpy(basename, ptr[x + 1]);

        strcpy(nodeType, "F");
    }

    int i;
    bool pass = true;
    bool path = false;
    for (i = 0; i < len; i++)
    {
        if (strcmp(fs[i].basename, basename) == 0 && strcmp(fs[i].parentFolder, parentFolder) == 0)
        {
            printf("\e[35mError, File already exists\n");
            pass = false;
            break;
        }

        if (strcmp(fs[i].basename, basename) == 0 && strcmp(fs[i].nodeType, "D") == 0)
        {
            pass = false;
            printf("\e[35m%s\n is a directroy", basename);
            break;
        }
        if (strcmp(fs[i].pathname, newpathname) == 0)
        {
            path = true;
        }
    }

    if (pass && path)
    {
        struct entity p;
        strcpy(p.pathname, newpathname);
        strcpy(p.nodeType, nodeType);
        strcpy(p.basename, basename);
        strcpy(p.parentFolder, parentFolder);
        addToFs(p);
    }
    lastPointer = 0;
    strcpy(newpathname, "");
    strcpy(basename, "");
    strcpy(parentFolder, "");
    strcpy(nodeType, "");
}
void rm(char pathname[20])
{

    //code here
    char *newpathname[200];
    char **ptr = paths;

    char *basename[100];
    char *parentFolder[100];
    char *nodeType[10];
    int x;
    bool first = true;
    int f;
    f = lastPointer;
    char *temp[100];

    if (f == 0)

    {
        strcpy(temp, "/");
        strcat(temp, ptr[0]);
        strcpy(newpathname, temp);
        strcpy(basename, ptr[0]);
        strcpy(parentFolder, "/");
        strcpy(nodeType, "D");
    }
    else
    {
        for (x = 0; x < f; x++)
        {
            if (ptr[x] != NULL)
            {
                if (first)
                {
                    strcpy(newpathname, "/");
                    strcat(newpathname, ptr[x]);
                    first = false;
                }
                else
                {

                    strcat(newpathname, "/");
                    strcat(newpathname, ptr[x]);
                }
                if (x < f)
                {
                    strcpy(parentFolder, newpathname);
                }
            }
            else
            {
                break;
            }
            strcpy(basename, ptr[x + 1]);
        }

        strcat(newpathname, "/");
        strcat(newpathname, ptr[f]);
    }

    int i;
    for (i = 0; i < len; i++)
    {
        if (strcmp(fs[i].basename, basename) == 0 && strcmp(fs[i].parentFolder, parentFolder) == 0)
        {
            if (strcmp(fs[i].nodeType, "F") == 0)
            {
                // delete element entity from file system
                int pos = i + 1;
                int size = len + 1;
                int j;

                for (j = pos - 1; j < size - 1; j++)
                {
                    fs[j] = fs[j + 1];
                }
                len--;
                return;
            }
            else
            {
                printf("\e[35m%s is not a File\n", basename);
                return;
            }
        }
    }
}
void save(char pathname[20])
{
    FILE *fp = fopen("myfileSystem", "w+");
    int k;
    for (k = 0; k < len; k++)
    {
        fprintf(fp, "%s,%s\n", fs[k].pathname, fs[k].nodeType);
    }
    fclose(fp);
    printf("[+]Saved filesystem\n");
}

void reload(char pathname[20])
{
    printf("[loading filesystem from file...]\n");
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("myfileSystem", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);
    int g = 1;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        char string[2000];
        strcpy(string, line);
        char *token = strtok(string, ",");

        char path[100];
        char type[50];
        bool first = true;
        while (token != NULL)
        {
            if (first)
            {
                strcpy(path, token);
                first = false;
            }
            else
            {
                strcpy(type, token);
            }

            token = strtok(NULL, ",");
        }
        path[strcspn(path, "\n")] = 0;
        type[strcspn(type, "\n")] = 0;
        printf("\e[32m[%d]%s --> [%s]\n", g, path, type);
        if (strcmp(type, "D") == 0)
        {
            tokenize(path);
            mkdirf(paths);
        }
        else
        {
            if (strcmp(type, "F") == 0)
            {
                tokenize(path);
                create(paths);
            }
        }
        g++;
    }

    fclose(fp);
    if (line)
        free(line);
}

int findCommand(char *command)
{
    int i = 0;
    while (cmd[i])
    {
        if (!strcmp(command, cmd[i]))
            return i;
        i++;
    }

    return -1;
}

int tokenize(char *pathname)
{
    char **ptr = paths;
    char *s;
    s = strtok(pathname, "/");
    int n = 0;
    while (s)
    {
        ptr[n] = s;
        s = strtok(0, "/");
        n++;
    }
    lastPointer = n - 1;
}

void addToFs(struct entity p)
{
    char pathname[100];
    if (strcmp(p.pathname, "/") == 0)
    {
        strcpy(pathname, p.pathname);
        strcat(pathname, p.basename);
    }
    else
    {
        strcpy(pathname, p.pathname);
        strcat(pathname, "/");
        strcat(pathname, p.basename);
    }

    strcpy(fs[len].basename, p.basename);
    strcpy(fs[len].pathname, pathname);
    strcpy(fs[len].parentFolder, p.parentFolder);
    strcpy(fs[len].nodeType, p.nodeType);
    len++;
}

bool on = true;
void menu()
{
    while (on)
    {
        char line[128];

        int command;
        printf("[1]---Save filesystem\n");
        printf("[2]---Open shell\n");
        printf("[3]---Quit\n");
        scanf("%d", &command);

        if (command == 1)
        {
            save(cwd);
        }
        else
        {
            if (command == 2)
            {
                on = false;
                return;
            }
            else
            {
                if (command == 3)
                {
                    exit(0);
                }
                else
                {
                    printf("Enter a valid command\n");
                }
            }
        }
    }
}

int main(int argc, char const *argv[])
{
    /* code */
    int i;
    innitialize();
    int index;
    char line[128], command[16], pathname[64];
    printf("-----------------[Unix filesystem simulator]-------------------\n");
    while (1)
    {
        on = true;
        printf("\e[34m%s >> ", cwd);
        fgets(line, 128, stdin);
        line[strlen(line) - 1] = 0;
        sscanf(line, "%s %s", command, pathname);
        index = findCommand(command);
        switch (index)
        {
        case 0:
            tokenize(pathname);
            mkdirf(paths);
            break;
        case 1:
            tokenize(pathname);
            rmdirf(paths);
            break;
        case 2:
            ls(pathname);
            break;
        case 3:
            cd(pathname);
            break;
        case 4:
            pwd(pathname);
            break;
        case 5:
            tokenize(pathname);
            create(paths);
            break;
        case 6:
            rm(pathname);
            break;
        case 7:
            reload(pathname);
            break;
        case 8:
            save(pathname);
            break;
        case 10:
            return 0;
            break;
        case 9:
            menu();
            break;
        default:
            printf("\e[35m[!] Invalid command \"%s\" check documentation for details \n", command);
        }
    }
    return 0;
}