#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fdfs_client.h"

/**
 * @brief   利用fastdfs提供的代码改写下载代码
 *
 * @param conf_filename 客户端配置文件路径
 * @param local_file_id 上传文件时返回的ID
 * @param to_savefile   下载保存到本地文件路径
 * @param file_offset   下载文件时偏移的字节数
 * @param download_bytes    需要下载的文件的字节数，需要准确数值
 * @return int
 */
int fastdfs_download_file_1(const char *conf_filename, const char *local_file_id, char *to_savefile,
                            const int64_t file_offset, const int64_t download_bytes)
{
    // 与tracker的连接对象
    ConnectionInfo *pTrackerServer;

    // 存放各种函数的执行结果
    int result;
    char file_id[128];

    // 借助客户端配置文件初始化客户端
    if ((result = fdfs_client_init(conf_filename)) != 0)
    {
        return result;
    }

    // 获取与tracker的连接对象
    pTrackerServer = tracker_get_connection();
    if (pTrackerServer == NULL)
    {
        fdfs_client_destroy();
        return errno != 0 ? errno : ECONNREFUSED;
    }

    // 将传入的文件ID放到file_id中
    // snprintf(file_id, sizeof(file_id), "%s", local_filename);

    int64_t file_size = 0;

    result = storage_do_download_file1_ex(pTrackerServer,
                                          NULL, FDFS_DOWNLOAD_TO_FILE, local_file_id,
                                          file_offset, download_bytes,
                                          &to_savefile, NULL, &file_size);
    if (result != 0)
    {
        printf("download file fail, "
               "error no: %d, error info: %s\n",
               result, STRERROR(result));
    }

    // 断开连接，释放客户端
    tracker_disconnect_server_ex(pTrackerServer, true);
    fdfs_client_destroy();

    return 1;
}

int main(int argc, char *argv[])
{
    // fastdfs_download_file_1(const char *conf_filename, const char *local_file_id, const char *to_savefile,
    //                         const int64_t file_offset, const int64_t download_bytes);

    fastdfs_download_file_1(
        "/etc/fdfs/client.conf",
        "group1/M00/00/00/wKgvg2QWb6mAX98WAAAOfCxQj8w13066.c",
        "./download.c",
        1027,
        2681); // 必须是文件确定的大小

    return 0;
}
