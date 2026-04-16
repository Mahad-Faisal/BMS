/* generated vector source file - do not edit */
#include "bsp_api.h"
/* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
#if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = iic_master_rxi_isr, /* IIC1 RXI (Receive data full) */
            [1] = iic_master_txi_isr, /* IIC1 TXI (Transmit data empty) */
            [2] = iic_master_tei_isr, /* IIC1 TEI (Transmit end) */
            [3] = iic_master_eri_isr, /* IIC1 ERI (Transfer error) */
            [4] = canfd_error_isr, /* CAN0 CHERR (Channel  error) */
            [5] = canfd_channel_tx_isr, /* CAN0 TX (Transmit interrupt) */
            [6] = canfd_common_fifo_rx_isr, /* CAN0 COMFRX (Common FIFO receive interrupt) */
            [7] = canfd_error_isr, /* CAN GLERR (Global error) */
            [8] = canfd_rx_fifo_isr, /* CAN RXF (Global receive FIFO interrupt) */
            [9] = sci_b_uart_rxi_isr, /* SCI3 RXI (Receive data full) */
            [10] = sci_b_uart_txi_isr, /* SCI3 TXI (Transmit data empty) */
            [11] = sci_b_uart_tei_isr, /* SCI3 TEI (Transmit end) */
            [12] = sci_b_uart_eri_isr, /* SCI3 ERI (Receive error) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_IIC1_RXI,GROUP0), /* IIC1 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TXI,GROUP1), /* IIC1 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_IIC1_TEI,GROUP2), /* IIC1 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_IIC1_ERI,GROUP3), /* IIC1 ERI (Transfer error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_CAN0_CHERR,GROUP4), /* CAN0 CHERR (Channel  error) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_CAN0_TX,GROUP5), /* CAN0 TX (Transmit interrupt) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_CAN0_COMFRX,GROUP6), /* CAN0 COMFRX (Common FIFO receive interrupt) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_CAN_GLERR,GROUP7), /* CAN GLERR (Global error) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_CAN_RXF,GROUP0), /* CAN RXF (Global receive FIFO interrupt) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_SCI3_RXI,GROUP1), /* SCI3 RXI (Receive data full) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TXI,GROUP2), /* SCI3 TXI (Transmit data empty) */
            [11] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TEI,GROUP3), /* SCI3 TEI (Transmit end) */
            [12] = BSP_PRV_VECT_ENUM(EVENT_SCI3_ERI,GROUP4), /* SCI3 ERI (Receive error) */
        };
        #endif
        #endif
