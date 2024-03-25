/*
 * Copyright (C) 2018-2022 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BOOT_H
#define BOOT_H

#include <multiboot_std.h>
#include <efi.h>
#include <vm_configurations.h>

/* TODO: MAX_MMAP_ENTRIES shall be config by config tool, and same as E820_MAX_ENTRIES */
#define MAX_MMAP_ENTRIES		32U  /**< Max memory map array size. */

#define MAX_BOOTARGS_SIZE		2048U  /**< Max command line size in byte. */
#define MAX_LOADER_NAME_SIZE		32U  /**< Max loader name size in byte. */
#define MAX_PROTOCOL_NAME_SIZE		16U  /**< Max protocol name size in byte. */
#define MAX_MOD_STRING_SIZE		2048U  /**< Max module name size in byte. */

/**
 * @brief Max modules ACRN supports.
 *
 * The modules in multiboot are: Pre-launched VM: kernel/ramdisk/acpi; Service VM: kernel/ramdisk
 */
#define MAX_MODULE_NUM			(3U * PRE_VM_NUM + 2U * SERVICE_VM_NUM)

/**
 * @brief ACPI module size in byte.
 *
 * The vACPI module size is fixed to 1MB
 */
#define ACPI_MODULE_SIZE                MEM_1M

/**
 * @brief Module of Acrn Boot Information.
 *
 * This struct represents a module that can be loaded into memory and run.
 * For example, ACRN needed service VM typecally is passed as a module.
 */
struct abi_module {
	void			*start;  /**< Module start address in HVA. */
	uint32_t		size;  /**< Module size in byte. */
	const char		string[MAX_MOD_STRING_SIZE];  /**< Module name, for example: Linux_bzImage. */
};

/* ABI memory map types, compatible to Multiboot/Multiboot2/E820; */
#define MMAP_TYPE_RAM		1U  /**< Available RAM which mainly used for memory paging by ACRN. */
#define MMAP_TYPE_RESERVED	2U  /**< Reserved area. */
#define MMAP_TYPE_ACPI_RECLAIM	3U  /**< Usable memory holding ACPI information. */
#define MMAP_TYPE_ACPI_NVS	4U  /**< Reserved memory which needs to be preserved on hibernation. */
#define MMAP_TYPE_UNUSABLE	5U  /**< Memory which is occupied by defective RAM modules. */

/**
 * @brief Memory Map of Acrn Boot Information.
 *
 * This struct represents a memory region, including its address, size, and type.
 *
 */
struct abi_mmap {
	uint64_t		baseaddr;  /**< Memory start address. */
	uint64_t		length;  /**< Memory length in byte. */
	uint32_t		type;  /**< Memory type. Must be one of MMAP_TYPE_*. */
};

/**
 * @brief Acrn Boot Information.
 *
 * There may be many protocols to boot the system, then pass control to ACRN.
 * This struct is used to save different protocols' important boot information in an unified format.
 */
struct acrn_boot_info {

	/**
	 * @brief Boot protocol, for example: Multiboot, Multiboot2, and others.
	 */
	char			protocol_name[MAX_PROTOCOL_NAME_SIZE];
	const char		cmdline[MAX_BOOTARGS_SIZE];  /**< Boot command line. */
	/**
	 * @brief Bootloader that implements the protocol. For multiboot protocol, GRUB is a possiable loader.
	 */
	const char		loader_name[MAX_LOADER_NAME_SIZE];

	uint32_t		mods_count;  /**< Module count. */
	struct abi_module	mods[MAX_MODULE_NUM];  /**< Module data structs. */

	uint32_t		mmap_entries;  /**< Memory map count. */
	struct abi_mmap		mmap_entry[MAX_MMAP_ENTRIES];  /**< Memory map data structs. */

	/**
	 * @brief ACPI Root System Description Pointer.
	 * For Multiboot2 protocol, the RSDP may ready and ACRN uses it directly.
	 * If protocol does not contain RSDP, ACRN will detects it in special memory regions.
	 */
	const void		*acpi_rsdp_va;
	struct efi_info		uefi_info;  /**< UEFI information. Typecally available in Multiboot2 protocol. */
};

/**
 * @brief Check if system boot from UEFI.
 * @param abi Pointer to acrn boot information.
 * @return true if boot from UEFI, or false if not.
 */
static inline bool boot_from_uefi(struct acrn_boot_info *abi)
{
	return !((abi->uefi_info.systab == 0U) && (abi->uefi_info.systab_hi == 0U));
}

/**
 * @brief Get memory range of all modules according to get_acrn_boot_info().
 * @param[out] p_start Pointer to receive lowest memory address.
 * @param[out] p_end Pointer to receive highest memory address.
 * @return None
 * @pre (p_start != NULL) && (p_end != NULL)
 * @post p_start and p_end are filled with right addresses if presents one or more modules, 
 * or (*p_start = ~0UL) && (*p_end = 0).
 */
void get_boot_mods_range(uint64_t *p_start, uint64_t *p_end);

/**
 * @brief Initialize multiboot information.
 * @param registers Point to a 2-element array which means multiboot magic number and MBI.
 * @return Initialization status.
 * @retval 0 No error.
 * @retval -ENODEV Unsupported multiboot version.
 */
int32_t init_multiboot_info(uint32_t *registers);

/**
 * @brief Initialize acrn boot information.
 * @param registers Point to a 2-element array which means multiboot magic number and MBI.
 * @return None
 */
void init_acrn_boot_info(uint32_t *registers);
/**
 * @brief Validate acrn boot information.
 * @param abi Pointer to acrn boot information.
 * @return Validation status.
 * @retval 0 No error.
 * @retval -EINVAL The abi is not a valid acrn boot information.
 */
int32_t sanitize_acrn_boot_info(struct acrn_boot_info *abi);
/**
 * @brief There is an instance of acrn boot information, and this function return its pointer.
 * @return Pointer to acrn boot information.
 * @post retval != NULL
 */
struct acrn_boot_info *get_acrn_boot_info(void);

/**
 * @brief Find module in acrn boot information by tag.
 * @return Pointer to module if found, or NULL.
 * @pre abi != NULL && tag != NULL
 */
struct abi_module *get_mod_by_tag(const struct acrn_boot_info *abi, const char *tag);
#endif	/* BOOT_H */
