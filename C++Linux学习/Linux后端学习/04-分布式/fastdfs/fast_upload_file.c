#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include "fdfs_client.h"

/**
 * @brief 				使用框架提供的方式上传文件并返回文件ID
 *
 * @param confFile 		客户端配置文件路径
 * @param uploadfile 	需要上传的文件所在本地的路径
 * @param fileID 		返回参数，文件在服务器中的ID
 * @return int 			执行成功与否
 */
int upload_file_1(const char *confFile, const char *uploadfile, char *fileID)
{
    char group_name[FDFS_GROUP_NAME_MAX_LEN + 1];
    ConnectionInfo *pTrackerServer;
    int result;
    int store_path_index;
    ConnectionInfo storageServer;

    if ((result = fdfs_client_init(confFile)) != 0)
    {
        return result;
    }

    pTrackerServer = tracker_get_connection();
    if (pTrackerServer == NULL)
    {
        fdfs_client_destroy();
        return errno != 0 ? errno : ECONNREFUSED;
    }

    *group_name = '\0';
    if ((result = tracker_query_storage_store(pTrackerServer,
                                              &storageServer, group_name, &store_path_index)) != 0)
    {
        fdfs_client_destroy();
        fprintf(stderr, "tracker_query_storage fail, "
                        "error no: %d, error info: %s\n",
                result, STRERROR(result));
        return result;
    }

    result = storage_upload_by_filename1(pTrackerServer,
                                         &storageServer, store_path_index,
                                         uploadfile, NULL,
                                         NULL, 0, group_name, fileID);
    if (result == 0)
    {
        printf("%s\n", fileID);
    }
    else
    {
        fprintf(stderr, "upload file fail, "
                        "error no: %d, error info: %s\n",
                result, STRERROR(result));
    }

    tracker_disconnect_server_ex(pTrackerServer, true);
    fdfs_client_destroy();

    return result;
}

/**
 * @brief 				使用执行命令的方式上传文件
 *
 * @param confFile 		客户端配置文件所在路径
 * @param uploadfile 	上传的文件
 * @param fileID 		返回的文件ID
 * @param size 			fileID的长度
 */
void upload_file_2(const char *confFile, const char *uploadfile, char *fileID, int size)
{
    // 1、创建匿名管道
    int fd[2] = {0};
    int ret = pipe(fd);
    if (ret == -1)
    {
        perror("pipe error: ");
        exit(0);
    }

    // 2、创建子进程
    pid_t pid = fork();

    if (pid == 0)
    {
        // 子进程
        // 将标准输出重定向到管道写端, 输出上传之后的文件ID
        dup2(fd[1], STDOUT_FILENO);
        // 关闭读端
        close(fd[0]);
        // 执行命令
        execlp("fdfs_upload_file", "fdfs_upload_file", confFile, uploadfile, NULL);
        perror("execlp: ");
    }
    else
    {
        // 父进程，读管道。接受上传之后的文件ID
        // 关闭写端
        close(fd[1]);
        read(fd[0], fileID, size);
        // 回收子进程
        wait(NULL);
    }
}

/**
 * @brief 测试两种上传文件的方式
 *
 * @return int
 */
int main(void)
{
    char fileID[1024] = {0};

    // 1、通过框架提供的方式上传问题
    // upload_file_1("/etc/fdfs/client.conf", "fastdfs_upload_file.c", fileID);
    // printf("上传之后的文件ID: %s\n", fileID);

    //---------------------------------------------

    // 2、通过命令方式
    memset(fileID, 0x0, sizeof fileID);
    upload_file_2("/etc/fdfs/client.conf", "fastdfs_upload_file.c", fileID, sizeof fileID);
    printf("上传之后的文件ID: %s\n", fileID);
}