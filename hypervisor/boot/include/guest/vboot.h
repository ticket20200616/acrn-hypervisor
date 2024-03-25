/*
 * Copyright (C) 2019-2022 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VBOOT_H

#define VBOOT_H

/**
 * @brief Initialize VM boot information for VM creation.
 * @param[inout] vm pointer to a vm descriptor
 *
 * @retval 0 on success
 * @retval -EINVAL on invalid parameters
 *
 * @pre vm != NULL
 */
int32_t init_vm_boot_info(struct acrn_vm *vm);
/**
 * @brief Load sw module info to VM.
 * @param[out] vm Pointer to loaded VM.
 * @param sw_module Pointer to sw_module_info.
 * @return None
 * @pre sw_module != NULL
 */
void load_sw_module(struct acrn_vm *vm, struct sw_module_info *sw_module);

#ifdef CONFIG_GUEST_KERNEL_BZIMAGE
/**
 * @brief Load bzImage to VM.
 * @param[inout] vm Pointer to loaded VM.
 *
 * @retval 0 on success
 * @retval -ENOMEM on not enough memory space
 *
 * @pre vm != NULL
 *
 */
int32_t bzimage_loader(struct acrn_vm *vm);
#endif
#ifdef CONFIG_GUEST_KERNEL_RAWIMAGE
int32_t rawimage_loader(struct acrn_vm *vm);
#endif
#ifdef CONFIG_GUEST_KERNEL_ELF
int32_t elf_loader(struct acrn_vm *vm);
#endif

#endif /* end of include guard: VBOOT_H */
