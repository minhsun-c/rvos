#include "vga.h"
#include "defs.h"
#include "nyancat-frame.h"
#include "types.h"

// Register structure (vport, index, data)
typedef struct {
    uint8_t vport;
    uint8_t index;
    uint8_t data;
} VgaReg;

static const VgaReg MODE_13_REGS[] = {
    {0xC2, 0x00, 0x63}, {0xD4, 0x11, 0x0E}, {0xD4, 0x00, 0x5F},
    {0xD4, 0x01, 0x4F}, {0xD4, 0x02, 0x50}, {0xD4, 0x03, 0x82},
    {0xD4, 0x04, 0x54}, {0xD4, 0x05, 0x80}, {0xD4, 0x06, 0xBF},
    {0xD4, 0x07, 0x1F}, {0xD4, 0x08, 0x00}, {0xD4, 0x09, 0x41},
    {0xD4, 0x10, 0x9C}, {0xD4, 0x11, 0x8E}, {0xD4, 0x12, 0x8F},
    {0xD4, 0x13, 0x28}, {0xD4, 0x14, 0x40}, {0xD4, 0x15, 0x96},
    {0xD4, 0x16, 0xB9}, {0xD4, 0x17, 0xA3}, {0xC4, 0x01, 0x01},
    {0xC4, 0x02, 0x0F}, {0xC4, 0x04, 0x0E}, {0xCE, 0x00, 0x00},
    {0xCE, 0x05, 0x40}, {0xCE, 0x06, 0x05}, {0xC0, 0x30, 0x41},
    {0xC0, 0x33, 0x00}};

void set_palette(void)
{
    volatile uint8_t *vga_port_base = (volatile uint8_t *) VGA_REG_BASE;

    // Set Palette
    *(vga_port_base + VGA_PORT_PEL_MSK) = 0xFF;
    *(vga_port_base + VGA_PORT_PEL_ADR) = 0x00;
    volatile uint8_t *p_data = vga_port_base + VGA_PORT_PEL_DAT;

#ifdef VGA_NYANCAT_TEST
    for (int i = 0; i < 14; i++) {
        uint8_t p = nyancat_palette[i];

        // Extract 2-bit components and scale to 6-bit top significance
        uint8_t r = ((p >> 4) & 0x03) << 4;
        uint8_t g = ((p >> 2) & 0x03) << 4;
        uint8_t b = (p & 0x03) << 4;

        *p_data = r;
        *p_data = g;
        *p_data = b;
    }
#else
    /* Use the Full 256-color palette */
    for (int i = 0; i < 256; i++) {
        uint32_t rgb = PALETTE[i];
        // Shift right by 2 to convert 8-bit to 6-bit for VGA DAC
        *p_data = (uint8_t) ((rgb >> 16) >> 2);  // R
        *p_data = (uint8_t) ((rgb >> 8) >> 2);   // G
        *p_data = (uint8_t) ((rgb >> 0) >> 2);   // B
    }
#endif
}

/**
 * @brief Configures the VGA hardware registers for Mode 13h.
 *
 * This function iterates through a predefined set of register values to
 * establish a 320x200 256-color linear framebuffer. It handles the specific
 * handshaking requirements for different VGA sub-controllers, including the
 * Attribute Controller's internal flip-flop reset.
 */
void set_mode13(void)
{
    /* VGA registers are mapped at a specific offset from the I/O base address
     */
    volatile uint8_t *vga_port_base = (volatile uint8_t *) VGA_REG_BASE;

    for (int i = 0; i < 28; i++) {
        uint8_t vport = MODE_13_REGS[i].vport;
        uint8_t index = MODE_13_REGS[i].index;
        uint8_t data = MODE_13_REGS[i].data;

        volatile uint8_t *port = vga_port_base + vport;

        /* The Attribute Controller uses a single port for both index and data
         */
        if (vport == 0xC0) {
            /* Reading 0x3DA resets the internal flip-flop to 'Address' mode */
            (void) *(vga_port_base + 0xDA);
            /* First write sets the register index */
            *port = index;
            /* Second write sets the register data */
            *port = data;
        }
        /* Miscellaneous Output is a standalone write-only register */
        else if (vport == 0xC2) {
            *port = data;
        }
        /* Sequencer, CRTC, and Graphics controllers use Index/Data port pairs
         */
        else {
            /* Write the index to the base port */
            *port = index;
            /* Write the data to the immediately following port (+1) */
            *(port + 1) = data;
        }
    }
}

/**
 * @brief Initializes the VGA controller via PCI and sets up Mode 13h.
 * * This function performs high-level hardware orchestration:
 * 1. Configures PCI Base Address Registers (BARs) to map the device into
 * the system's physical memory map.
 * 2. Enables Memory and I/O access via the PCI Command register.
 * 3. Invokes low-level register configuration for 320x200 256-color mode.
 * 4. Resets the Attribute Controller flip-flop to unblank the display.
 */
void vga_init()
{
    /* Access the VGA device's configuration space via PCI ECAM */
    volatile uint32_t *pci_conf = (volatile uint32_t *) VGA_PCI_CONF;

    /* Set the base address for the linear framebuffer (Pixel Memory) */
    pci_conf[PCI_REG_BAR0 / 4] = (uint32_t) VGA_FB_BASE;

    /* Set the base address for the I/O register mapping window */
    pci_conf[PCI_REG_BAR4 / 4] = (uint32_t) VGA_IO_BASE;

    /* Enable I/O space, Memory space, and Bus Mastering in the PCI Command
     * Register */
    pci_conf[PCI_REG_COMMAND / 4] |= 0x07;

    /* Ensure PCI configuration writes are committed before accessing I/O ports
     */
    __asm__ volatile("fence w, o" ::: "memory");

    /* Configure internal VGA registers for 320x200 graphics mode */
    set_mode13();

    /* Load the selected color palette into the DAC */
    set_palette();

    /* Access the VGA register block (typically starting at 0x3C0) */
    volatile uint8_t *vga_io = (uint8_t *) VGA_REG_BASE;

    /**
     * Reset the Attribute Controller flip-flop.
     * Reading the Input Status Register 1 (0x3DA) forces the next write
     * to 0x3C0 to be interpreted as an Address/Index rather than Data.
     */
    (void) vga_io[VGA_PORT_STATUS - 0xC0];

    /**
     * Unblank the screen.
     * Writing 0x20 to the Attribute Address register sets bit 5,
     * which enables the display output.
     */
    vga_io[VGA_PORT_ATTR - 0xC0] = 0x20;
}