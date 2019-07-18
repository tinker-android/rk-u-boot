//
// Created by lili on 19-7-18.
//
#include <common.h>
#include <android_image.h>
#include <malloc.h>

static int do_android_image_test(cmd_tbl_t *cmdtp, int flag, int argc,
                         char * const argv[]){
    struct andr_img_hdr *hdr;
    struct blk_desc *dev_desc;
    disk_partition_t part_info;
    int ret = 0;
    u32 blk_cnt;

    printf("%s\n", __func__);

    dev_desc = blk_get_dev(argv[1], (int) simple_strtoul(argv[2], NULL, 16));
    if(dev_desc == NULL) return CMD_RET_FAILURE;

    ret = part_get_info_by_name(dev_desc, "boot", &part_info);
    if(ret < 0){
        return CMD_RET_FAILURE;
    }

    blk_cnt = DIV_ROUND_UP(sizeof(*hdr), part_info.blksz);
    hdr = memalign(ARCH_DMA_MINALIGN, part_info.blksz * blk_cnt);
    if (!hdr) {
        printf("%s: out of memory!\n", __func__);
        return CMD_RET_FAILURE;
    }

    ret = blk_dread(dev_desc, part_info.start, blk_cnt, hdr);
    if (ret != blk_cnt) {
        printf("%s: failed to read %s hdr!\n", __func__, part_info.name);
        goto out;
    }

    return android_image_check_header(hdr);

out:
    free(hdr);
    return CMD_RET_FAILURE;
}

U_BOOT_CMD(
        androidtest, 3, 0,    do_android_image_test,
"Test if storage media have android image",
""
);