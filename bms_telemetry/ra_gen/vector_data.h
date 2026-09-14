/* generated vector header file - do not edit */
#ifndef VECTOR_DATA_H
#define VECTOR_DATA_H
#ifdef __cplusplus
        extern "C" {
        #endif
/* Number of interrupts allocated */
#ifndef VECTOR_DATA_IRQ_COUNT
#define VECTOR_DATA_IRQ_COUNT    (13)
#endif
/* ISR prototypes */
void iic_master_rxi_isr(void);
void iic_master_txi_isr(void);
void iic_master_tei_isr(void);
void iic_master_eri_isr(void);
void canfd_error_isr(void);
void canfd_channel_tx_isr(void);
void canfd_common_fifo_rx_isr(void);
void canfd_rx_fifo_isr(void);
void sci_b_uart_rxi_isr(void);
void sci_b_uart_txi_isr(void);
void sci_b_uart_tei_isr(void);
void sci_b_uart_eri_isr(void);

/* Vector table allocations */
#define VECTOR_NUMBER_IIC1_RXI ((IRQn_Type) 0) /* IIC1 RXI (Receive data full) */
#define IIC1_RXI_IRQn          ((IRQn_Type) 0) /* IIC1 RXI (Receive data full) */
#define VECTOR_NUMBER_IIC1_TXI ((IRQn_Type) 1) /* IIC1 TXI (Transmit data empty) */
#define IIC1_TXI_IRQn          ((IRQn_Type) 1) /* IIC1 TXI (Transmit data empty) */
#define VECTOR_NUMBER_IIC1_TEI ((IRQn_Type) 2) /* IIC1 TEI (Transmit end) */
#define IIC1_TEI_IRQn          ((IRQn_Type) 2) /* IIC1 TEI (Transmit end) */
#define VECTOR_NUMBER_IIC1_ERI ((IRQn_Type) 3) /* IIC1 ERI (Transfer error) */
#define IIC1_ERI_IRQn          ((IRQn_Type) 3) /* IIC1 ERI (Transfer error) */
#define VECTOR_NUMBER_CAN0_CHERR ((IRQn_Type) 4) /* CAN0 CHERR (Channel  error) */
#define CAN0_CHERR_IRQn          ((IRQn_Type) 4) /* CAN0 CHERR (Channel  error) */
#define VECTOR_NUMBER_CAN0_TX ((IRQn_Type) 5) /* CAN0 TX (Transmit interrupt) */
#define CAN0_TX_IRQn          ((IRQn_Type) 5) /* CAN0 TX (Transmit interrupt) */
#define VECTOR_NUMBER_CAN0_COMFRX ((IRQn_Type) 6) /* CAN0 COMFRX (Common FIFO receive interrupt) */
#define CAN0_COMFRX_IRQn          ((IRQn_Type) 6) /* CAN0 COMFRX (Common FIFO receive interrupt) */
#define VECTOR_NUMBER_CAN_GLERR ((IRQn_Type) 7) /* CAN GLERR (Global error) */
#define CAN_GLERR_IRQn          ((IRQn_Type) 7) /* CAN GLERR (Global error) */
#define VECTOR_NUMBER_CAN_RXF ((IRQn_Type) 8) /* CAN RXF (Global receive FIFO interrupt) */
#define CAN_RXF_IRQn          ((IRQn_Type) 8) /* CAN RXF (Global receive FIFO interrupt) */
#define VECTOR_NUMBER_SCI3_RXI ((IRQn_Type) 9) /* SCI3 RXI (Receive data full) */
#define SCI3_RXI_IRQn          ((IRQn_Type) 9) /* SCI3 RXI (Receive data full) */
#define VECTOR_NUMBER_SCI3_TXI ((IRQn_Type) 10) /* SCI3 TXI (Transmit data empty) */
#define SCI3_TXI_IRQn          ((IRQn_Type) 10) /* SCI3 TXI (Transmit data empty) */
#define VECTOR_NUMBER_SCI3_TEI ((IRQn_Type) 11) /* SCI3 TEI (Transmit end) */
#define SCI3_TEI_IRQn          ((IRQn_Type) 11) /* SCI3 TEI (Transmit end) */
#define VECTOR_NUMBER_SCI3_ERI ((IRQn_Type) 12) /* SCI3 ERI (Receive error) */
#define SCI3_ERI_IRQn          ((IRQn_Type) 12) /* SCI3 ERI (Receive error) */
/* The number of entries required for the ICU vector table. */
#define BSP_ICU_VECTOR_NUM_ENTRIES (13)

#ifdef __cplusplus
        }
        #endif
#endif /* VECTOR_DATA_H */
