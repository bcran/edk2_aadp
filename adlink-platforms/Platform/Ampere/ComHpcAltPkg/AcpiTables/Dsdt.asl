/** @file

  Copyright (c) 2020 - 2021, Ampere Computing LLC. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

DefinitionBlock("Dsdt.aml", "DSDT", 0x02, "Ampere", "Jade", 1) {
  //
  // Board Model
  Name(\BDMD, "COM-HPC-ALT Board")
  Name(TPMF, 0)  // TPM presence
  Name(AERF, 0)  // PCIe AER Firmware-First
  Scope(\_SB) {

    Include ("CPU.asi")
    Include ("PMU.asi")

    //
    // Hardware Monitor
    Device(HM00) {
      Name(_HID, "APMC0D29")
      Name(_UID, "HWM0")
      Name(_DDN, "HWM0")
      Name(_CCA, ONE)
      Name(_STR, Unicode("Hardware Monitor Device"))
      Method(_STA, 0, NotSerialized) {
        return (0xF)
      }
      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package() {
          Package() {"pcc-channel", 14}
        }
      })
    }

    //
    // Hardware Monitor
    Device(HM01) {
      Name(_HID, "APMC0D29")
      Name(_UID, "HWM1")
      Name(_DDN, "HWM1")
      Name(_CCA, ONE)
      Name(_STR, Unicode("Hardware Monitor Device"))
      Method(_STA, 0, NotSerialized) {
        return (0xF)
      }
      Name (_DSD, Package () {
        ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
        Package() {
          Package() {"pcc-channel", 29}
        }
      })
    }

    //
    // Hardware Monitor
    Device(HM02) {
      Name(_HID, "AMPC0005")
      Name(_UID, "HWM2")
      Name(_DDN, "HWM2")
      Name(_CCA, ONE)
      Name(_STR, Unicode("AC01 SoC Hardware Monitor Device"))
      Method(_STA, 0, NotSerialized) {
        return (0xF)
      }
      Name(_CRS, ResourceTemplate() {
        QWordMemory (
          ResourceProducer,     // ResourceUsage
          PosDecode,            // Decode
          MinFixed,             // IsMinFixed
          MaxFixed,             // IsMaxFixed
          Cacheable,            // Cacheable
          ReadWrite,            // ReadAndWrite
          0x0000000000000000,   // AddressGranularity - GRA
          0x0000000088900000,   // AddressMinimum - MIN
          0x000000008891FFFF,   // AddressMaximum - MAX
          0x0000000000000000,   // AddressTranslation - TRA
          0x0000000000020000    // RangeLength - LEN
        )
      })
    }

    //
    // Hardware Monitor
    Device(HM03) {
      Name(_HID, "AMPC0005")
      Name(_UID, "HWM3")
      Name(_DDN, "HWM3")
      Name(_CCA, ONE)
      Name(_STR, Unicode("AC01 SoC Hardware Monitor Device"))
      Method(_STA, 0, NotSerialized) {
        return (0xF)
      }
      Name(_CRS, ResourceTemplate() {
        QWordMemory (
          ResourceProducer,     // ResourceUsage
          PosDecode,            // Decode
          MinFixed,             // IsMinFixed
          MaxFixed,             // IsMaxFixed
          Cacheable,            // Cacheable
          ReadWrite,            // ReadAndWrite
          0x0000000000000000,   // AddressGranularity - GRA
          0x0000000088920000,   // AddressMinimum - MIN
          0x000000008893FFFF,   // AddressMaximum - MAX
          0x0000000000000000,   // AddressTranslation - TRA
          0x0000000000020000    // RangeLength - LEN
        )
      })
    }

    //
    // DesignWare I2C on AHBC bus
    Device(I2C5) {
      Name(_HID, "APMC0D0F")
      Name(_UID, 5)
      Name(_STR, Unicode("Altra I2C Device"))
      Method(_STA, 0, NotSerialized) {
        return (0x0f)
      }
      Name(_CCA, ONE)
      Name(_CRS, ResourceTemplate() {
        QWordMemory (
          ResourceProducer,     // ResourceUsage
          PosDecode,            // Decode
          MinFixed,             // IsMinFixed
          MaxFixed,             // IsMaxFixed
          NonCacheable,         // Cacheable
          ReadWrite,            // ReadAndWrite
          0x0000000000000000,   // AddressGranularity - GRA
          0x00001000026C0000,   // AddressMinimum - MIN
          0x00001000026CFFFF,   // AddressMaximum - MAX
          0x0000000000000000,   // AddressTranslation - TRA
          0x0000000000010000    // RangeLength - LEN
        )
        Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 106 }
      })
    }

    //
    // DesignWare I2C on AHBC bus
    Device(I2C6) {
      Name(_HID, "APMC0D0F")
      Name(_UID, 6)
      Name(_STR, Unicode("Altra I2C Device"))
      Method(_STA, 0, NotSerialized) {
        return (0x0f)
      }
      Name(_CCA, ONE)
      Name(_CRS, ResourceTemplate() {
        QWordMemory (
          ResourceProducer,     // ResourceUsage
          PosDecode,            // Decode
          MinFixed,             // IsMinFixed
          MaxFixed,             // IsMaxFixed
          NonCacheable,         // Cacheable
          ReadWrite,            // ReadAndWrite
          0x0000000000000000,   // AddressGranularity - GRA
          0x0000100002750000,   // AddressMinimum - MIN
          0x000010000275FFFF,   // AddressMaximum - MAX
          0x0000000000000000,   // AddressTranslation - TRA
          0x0000000000010000    // RangeLength - LEN
        )
        Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 107 }
      })
    }

    //
    // DesignWare I2C on AHBC bus
    Device(I2C9) {
      Name(_HID, "APMC0D0F")
      Name(_UID, 9)
      Name(_STR, Unicode("Altra I2C Device"))
      Method(_STA, 0, NotSerialized) {
        return (0x0f)
      }
      Name(_CCA, ONE)
      Name(_CRS, ResourceTemplate() {
        QWordMemory (
          ResourceProducer,     // ResourceUsage
          PosDecode,            // Decode
          MinFixed,             // IsMinFixed
          MaxFixed,             // IsMaxFixed
          NonCacheable,         // Cacheable
          ReadWrite,            // ReadAndWrite
          0x0000000000000000,   // AddressGranularity - GRA
          0x0000100002780000,   // AddressMinimum - MIN
          0x000010000278FFFF,   // AddressMaximum - MAX
          0x0000000000000000,   // AddressTranslation - TRA
          0x0000000000010000    // RangeLength - LEN
        )
        Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 110 }
      })
      Device (IPI) {
        Name(_HID, "AMPC0004")
        Name(_CID, "IPI0001")
        Name(_STR, Unicode("IPMI_SSIF"))
        Name(_UID, 0)
        Name(_CCA, ONE)
        Method(_STA, 0, NotSerialized) {
          Return (0x0f)
        }
        Method(_IFT) {
          Return(0x04) // IPMI SSIF
        }
        Method(_ADR) {
          Return(0x10) // SSIF slave address
        }
        Method(_SRV) {
          Return(0x0200) // IPMI Specification Revision
        }
        Name(_CRS, ResourceTemplate ()
        {
          I2cSerialBusV2 (0x0010, ControllerInitiated, 0x00061A80,
            AddressingMode7Bit, "\\_SB.I2C9",
            0x00, ResourceConsumer,, Exclusive,
            // Vendor specific data:
            // "BMC0",
            // Flags (2 bytes): SMBUS variable length (Bit 0), Read Checksum (Bit 1), Verify Checksum (Bit 2)
            RawDataBuffer () { 0x42, 0x4D, 0x43, 0x30, 0x7, 0x0 }
            )
        })
      }
      Name(SSCN, Package() { 0x3E2, 0x47D, 0 })
      Name(FMCN, Package() { 0xA4, 0x13F, 0 })
    }

    //
    // Report APEI Errors to GHES via SCI notification.
    // SCI notification requires one GED and one HED Device
    //     GED = Generic Event Device (ACPI0013)
    //     HED = Hardware Error Device (PNP0C33)
    //
    Device(GED0) {
        Name(_HID, "ACPI0013")
        Name(_UID, Zero)
        Method(_STA) {
          Return (0xF)
        }
        Name(_CRS, ResourceTemplate () {
          Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 84 } // GHES
          Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 44 } // PCIe Hot Plug Doorbell Insertion & Ejection (DBNS4 -> GIC-IRQS44)
        })

        // @DRAM base address 0x88970000
        OperationRegion(MHPP, SystemMemory, 0x88970000, 960)
        Field (MHPP, DWordAcc, NoLock, Preserve) {
            A020, 32, // 0. S0 RCA2.0
            offset (24),
            A030, 32, // 1. S0 RCA3.0
            offset (48),
            B000, 32, // 2. S0 RCB0.0
            offset (72),
            B002, 32, // 3. S0 RCB0.2
            offset (96),
            B004, 32, // 4. S0 RCB0.4
            offset (120),
            B006, 32, // 5. S0 RCB0.6
            offset (144),
            B010, 32, // 6. S0 RCB1.0
            offset (168),
            B012, 32, // 7. S0 RCB1.2
            offset (192),
            B014, 32, // 8. S0 RCB1.4
            offset (216),
            B016, 32, // 9. S0 RCB1.6
            offset (240),
            B020, 32, // 10. S0 RCB2.0
            offset (264),
            B022, 32, // 11. S0 RCB2.2
            offset (288),
            B024, 32, // 12. S0 RCB2.4
            offset (312),
            B030, 32, // 13. S0 RCB3.0
            offset (336),
            B034, 32, // 14. S0 RCB3.4
            offset (360),
            B036, 32, // 15. S0 RCB3.6
            offset (384),
            A120, 32, // 16. S1 RCA2.0
            offset (408),
            A121, 32, // 17. S1 RCA2.1
            offset (432),
            A122, 32, // 18. S1 RCA2.2
            offset (456),
            A123, 32, // 19. S1 RCA2.3
            offset (480),
            A130, 32, // 20. S1 RCA3.0
            offset (504),
            A132, 32, // 21. S1 RCA3.2
            offset (528),
            B100, 32, // 22. S1 RCB0.0
            offset (552),
            B104, 32, // 23. S1 RCB0.4
            offset (576),
            B106, 32, // 24. S1 RCB0.6
            offset (600),
            B110, 32, // 25. S1 RCB1.0
            offset (624),
            B112, 32, // 26. S1 RCB1.2
            offset (648),
            B114, 32, // 27. S1 RCB1.4
            offset (672),
            B120, 32, // 28. S1 RCB2.0
            offset (696),
            B122, 32, // 29. S1 RCB2.2
            offset (720),
            B124, 32, // 30. S1 RCB2.4
            offset (744),
            B126, 32, // 31. S1 RCB2.6
            offset (768),
            B130, 32, // 32. S1 RCB3.0
            offset (792),
            B132, 32, // 33. S1 RCB3.2
            offset (816),
            B134, 32, // 34. S1 RCB3.4
            offset (840),
            B136, 32, // 35. S1 RCB3.6
        }

        // @DBN4 agent base address for HP PCIe insertion/ejection event: 0x1000.0054.4000
        OperationRegion(DBN4, SystemMemory, 0x100000544010, 20)
        Field (DBN4, DWordAcc, NoLock, Preserve) {
          DOUT, 32, // event and PCIe port information at offset 0x10
          offset (0x10),
          STA4, 32, // interrupt status at offset 0x20
        }

        Method(_EVT, 1, Serialized) {
          Switch (ToInteger(Arg0)) {
            Case (84) { // GHES interrupt
              Notify (HED0, 0x80)
            }

            Case (44) { // doorbell notification (Insertion/ejection)
              local0 = DOUT & 0x00FF0000
              if (local0 == 0x00010000) {
                local0 = STA4 & 0xFFFFFFFF
                if (local0) {
                  Store(local0, STA4) // clear interrupt
                }
                local0 = A120 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIA.P2P1, 1) // insertion action
                }
                local0 = A121 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIA.P2P2, 1) // insertion action
                }
                local0 = A122 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIA.P2P3, 1) // insertion action
                }
                local0 = A123 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIA.P2P4, 1) // insertion action
                }
                local0 = B000 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI4.P2P1, 1) // insertion action
                }
                local0 = B002 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI4.P2P3, 1) // insertion action
                }
                local0 = B004 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI4.P2P5, 1) // insertion action
                }
                local0 = B006 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI4.P2P7, 1) // insertion action
                }
                local0 = B010 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI5.P2P1, 1) // insertion action
                }
                local0 = B012 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI5.P2P3, 1) // insertion action
                }
                local0 = B014 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI5.P2P5, 1) // insertion action
                }
                local0 = B016 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCI5.P2P7, 1) // insertion action
                }
                local0 = B104 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIC.P2P5, 1) // insertion action
                }
                local0 = B106 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIC.P2P7, 1) // insertion action
                }
                local0 = B110 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCID.P2P1, 1) // insertion action
                }
                local0 = B112 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCID.P2P3, 1) // insertion action
                }
                local0 = B120 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIE.P2P1, 1) // insertion action
                }
                local0 = B122 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIE.P2P3, 1) // insertion action
                }
                local0 = B124 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIE.P2P5, 1) // insertion action
                }
                local0 = B126 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIE.P2P7, 1) // insertion action
                }
                local0 = B130 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIF.P2P1, 1) // insertion action
                }
                local0 = B132 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIF.P2P3, 1) // insertion action
                }
                local0 = B134 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIF.P2P5, 1) // insertion action
                }
                local0 = B136 & 0xFF000000
                if (local0 == 0x01000000) {
                  Notify (\_SB.PCIF.P2P7, 1) // insertion action
                }
              }
              elseif (local0 == 0x00000000) {
                local0 = STA4 & 0xFFFFFFFF
                if (local0) {
                  Store(local0, STA4) // clear interrupt
                }
                local0 = A120 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIA.P2P1, 1) // ejection action
                }
                local0 = A121 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIA.P2P2, 1) // ejection action
                }
                local0 = A122 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIA.P2P3, 1) // ejection action
                }
                local0 = A123 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIA.P2P4, 1) // ejection action
                }
                local0 = B000 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI4.P2P1, 1) // ejection action
                }
                local0 = B002 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI4.P2P3, 1) // ejection action
                }
                local0 = B004 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI4.P2P5, 1) // ejection action
                }
                local0 = B006 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI4.P2P7, 1) // ejection action
                }
                local0 = B010 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI5.P2P1, 1) // ejection action
                }
                local0 = B012 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI5.P2P3, 1) // ejection action
                }
                local0 = B014 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI5.P2P5, 1) // ejection action
                }
                local0 = B016 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCI5.P2P7, 1) // ejection action
                }
                local0 = B104 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIC.P2P5, 1) // ejection action
                }
                local0 = B106 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIC.P2P7, 1) // ejection action
                }
                local0 = B110 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCID.P2P1, 1) // ejection action
                }
                local0 = B112 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCID.P2P3, 1) // ejection action
                }
                local0 = B120 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIE.P2P1, 1) // ejection action
                }
                local0 = B122 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIE.P2P3, 1) // ejection action
                }
                local0 = B124 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIE.P2P5, 1) // ejection action
                }
                local0 = B126 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIE.P2P7, 1) // ejection action
                }
                local0 = B130 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIF.P2P1, 1) // ejection action
                }
                local0 = B132 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIF.P2P3, 1) // ejection action
                }
                local0 = B134 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIF.P2P5, 1) // ejection action
                }
                local0 = B136 & 0xFF000000
                if (local0 == 0x00000000) {
                  Notify (\_SB.PCIF.P2P7, 1) // ejection action
                }
              }
            }
          }
        }
    }

    // Shutdown button using GED.
    Device(GED1) {
        Name(_HID, "ACPI0013")
        Name(_CID, "ACPI0013")
        Name(_UID, One)
        Method(_STA) {
          Return (0xF)
        }
        Name(_CRS, ResourceTemplate () {
          Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 327 }
        })
        OperationRegion(PDDR, SystemMemory, 0x1000027B0004, 4)
        Field(PDDR, DWordAcc, NoLock, Preserve) {
          STDI, 8
        }

        OperationRegion(INTE, SystemMemory, 0x1000027B0030, 4)
        Field(INTE, DWordAcc, NoLock, Preserve) {
          STDE, 8
        }

        OperationRegion(INTT, SystemMemory, 0x1000027B0034, 4)
        Field(INTT, DWordAcc, NoLock, Preserve) {
          TYPE, 8
        }

        OperationRegion(INTP, SystemMemory, 0x1000027B0038, 4)
        Field(INTP, DWordAcc, NoLock, Preserve) {
          POLA, 8
        }

        OperationRegion(INTS, SystemMemory, 0x1000027B003c, 4)
        Field(INTS, DWordAcc, NoLock, Preserve) {
          STDS, 8
        }

        OperationRegion(INTC, SystemMemory, 0x1000027B0040, 4)
        Field(INTC, DWordAcc, NoLock, Preserve) {
          SINT, 8
        }

        OperationRegion(INTM, SystemMemory, 0x1000027B0044, 4)
        Field(INTM, DWordAcc, NoLock, Preserve) {
          MASK, 8
        }

        Method(_INI, 0, NotSerialized) {
          // Set level type, low active (shutdown)
          Store (0x00, TYPE)
          Store (0x00, POLA)
          // Set Input type (shutdown)
          Store (0x00, STDI)
          // Enable interrupt (shutdown)
          Store (0x80, STDE)
          // Unmask the interrupt.
          Store (0x00, MASK)
        }
        Method(_EVT, 1, Serialized) {
          Switch (ToInteger(Arg0)) {
            Case (327) {
              if (And (STDS, 0x80)) {
                //Clear the interrupt.
                Store (0x80, SINT)
                // Notify OSPM the power button is pressed
                Notify (\_SB.PWRB, 0x80)
              }
            }
          }
        }
    }

    // Power button device description
    Device(PWRB) {
        Name(_HID, EISAID("PNP0C0C"))
        Name(_ADR, 0)
        Name(_UID, 0)
        Name(_CCA, ONE)
        Method(_STA, 0, Notserialized) {
            Return (0x0b)
        }
    }

    //
    // UART0 PL011
    Device(URT0) {
      Name(_HID, "ARMH0011")
      Name(_UID, 0)
      Name(_CCA, ONE)
      Method(_STA, 0, NotSerialized) {
        return (0x0f)
      }
      Name(_CRS, ResourceTemplate() {
        QWordMemory (
          ResourceProducer,     // ResourceUsage
          PosDecode,            // Decode
          MinFixed,             // IsMinFixed
          MaxFixed,             // IsMaxFixed
          NonCacheable,         // Cacheable
          ReadWrite,            // ReadAndWrite
          0x0000000000000000,   // AddressGranularity - GRA
          0x0000100002600000,   // AddressMinimum - MIN
          0x0000100002600FFF,   // AddressMaximum - MAX
          0x0000000000000000,   // AddressTranslation - TRA
          0x0000000000001000    // RangeLength - LEN
        )
        Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 98 }
      })
    } // UART0

    //
    // UART2 PL011
    Device(URT2) {
      Name(_HID, "ARMH0011")
      Name(_UID, 1)
      Name(_CCA, ONE)
      Method(_STA, 0, NotSerialized) {
        return (0x0f)
      }
      Name(_CRS, ResourceTemplate() {
        QWordMemory (
          ResourceProducer,     // ResourceUsage
          PosDecode,            // Decode
          MinFixed,             // IsMinFixed
          MaxFixed,             // IsMaxFixed
          NonCacheable,         // Cacheable
          ReadWrite,            // ReadAndWrite
          0x0000000000000000,   // AddressGranularity - GRA
          0x0000100002620000,   // AddressMinimum - MIN
          0x0000100002620FFF,   // AddressMaximum - MAX
          0x0000000000000000,   // AddressTranslation - TRA
          0x0000000000001000    // RangeLength - LEN
        )
        Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 100 }
      })
    } // UART1

    Device(HED0)
    {
        Name(_HID, EISAID("PNP0C33"))
        Name(_UID, Zero)
    }

    Device(NVDR) {
      Name(_HID, "ACPI0012")
      Method(_STA, 0, NotSerialized) {
        return (0xf)
      }
      Method (_DSM, 0x4, Serialized) {
        // Not support any functions for now
        Return (Buffer() {0})
      }
      Device (NVD1) {
        Name(_ADR, 0x0330)
        Name(SMRT, Buffer(13) {0})
        CreateDWordField(SMRT, 0, BSTA)
        CreateWordField(SMRT, 4, BHTH)
        CreateWordField(SMRT, 6, BTMP)
        CreateByteField(SMRT, 8, BETH)
        CreateByteField(SMRT, 9, BWTH)
        CreateByteField(SMRT, 10, BNLF)
        OperationRegion(BUF1, SystemMemory, 0x88980000, 16)
        Field (BUF1, DWordAcc, NoLock, Preserve) {
          STAT, 32, //Status
          HLTH, 16, //Module Health
          CTMP, 16, //Module Current Status
          ETHS, 8, //Error Threshold Status
          WTHS, 8, //Warning Threshold Status
          NVLF, 8, //NVM Lifetime
          ,     40 //Reserve
        }
        Method (_DSM, 0x4, Serialized) {
          //Accept only MSF Family type NVDIMM DSM functions
          If(LEqual(Arg0, ToUUID ("1ee68b36-d4bd-4a1a-9a16-4f8e53d46e05"))) {
            //Handle Func 0 query implemented commands
            If(LEqual(Arg2, 0)) {
              //Check revision and returned proper implemented commands
              //Support only health check for now
              Return (Buffer() {0x01, 0x08}) //Byte 0: 0x1
            }
            //Handle MSF DSM Func 11 Get Smart and Health Info
            If(LEqual(Arg2, 11)) {
              Store(\_SB.NVDR.NVD1.STAT, BSTA)
              Store(\_SB.NVDR.NVD1.HLTH, BHTH)
              Store(\_SB.NVDR.NVD1.CTMP, BTMP)
              Store(\_SB.NVDR.NVD1.ETHS, BETH)
              Store(\_SB.NVDR.NVD1.WTHS, BWTH)
              Store(\_SB.NVDR.NVD1.NVLF, BNLF)
              Return (SMRT)
            }
          }
          Return (Buffer() {0})
        }
        Method(_STA, 0, NotSerialized) {
          return (0xf)
        }
      }
      Device (NVD2) {
        Name(_ADR, 0x0770)
        Name(SMRT, Buffer(13) {0})
        CreateDWordField(SMRT, 0, BSTA)
        CreateWordField(SMRT, 4, BHTH)
        CreateWordField(SMRT, 6, BTMP)
        CreateByteField(SMRT, 8, BETH)
        CreateByteField(SMRT, 9, BWTH)
        CreateByteField(SMRT, 10, BNLF)
        OperationRegion(BUF1, SystemMemory, 0x88988000, 16)
        Field (BUF1, DWordAcc, NoLock, Preserve) {
          STAT, 32, //Status
          HLTH, 16, //Module Health
          CTMP, 16, //Module Current Status
          ETHS, 8, //Error Threshold Status
          WTHS, 8, //Warning Threshold Status
          NVLF, 8, //NVM Lifetime
          ,     40 //Reserve
        }
        Method (_DSM, 0x4, Serialized) {
          //Accept only MSF Family type NVDIMM DSM functions
          If(LEqual(Arg0, ToUUID ("1ee68b36-d4bd-4a1a-9a16-4f8e53d46e05"))) {
            //Handle Func 0 query implemented commands
            If(LEqual(Arg2, 0)) {
              //Check revision and returned proper implemented commands
              //Support only health check for now
              Return (Buffer() {0x01, 0x08}) //Byte 0: 0x1
            }
            //Handle MSF DSM Func 11 Get Smart and Health Info
            If(LEqual(Arg2, 11)) {
              Store(\_SB.NVDR.NVD2.STAT, BSTA)
              Store(\_SB.NVDR.NVD2.HLTH, BHTH)
              Store(\_SB.NVDR.NVD2.CTMP, BTMP)
              Store(\_SB.NVDR.NVD2.ETHS, BETH)
              Store(\_SB.NVDR.NVD2.WTHS, BWTH)
              Store(\_SB.NVDR.NVD2.NVLF, BNLF)
              Return (SMRT)
            }
          }
          Return (Buffer() {0})
        }
        Method(_STA, 0, NotSerialized) {
          return (0xf)
        }
      }
      Device (NVD3) {
        Name(_ADR, 0x1330)
        Name(SMRT, Buffer(13) {0})
        CreateDWordField(SMRT, 0, BSTA)
        CreateWordField(SMRT, 4, BHTH)
        CreateWordField(SMRT, 6, BTMP)
        CreateByteField(SMRT, 8, BETH)
        CreateByteField(SMRT, 9, BWTH)
        CreateByteField(SMRT, 10, BNLF)
        OperationRegion(BUF1, SystemMemory, 0xC0080000, 16)
        Field (BUF1, DWordAcc, NoLock, Preserve) {
          STAT, 32, //Status
          HLTH, 16, //Module Health
          CTMP, 16, //Module Current Status
          ETHS, 8, //Error Threshold Status
          WTHS, 8, //Warning Threshold Status
          NVLF, 8, //NVM Lifetime
          ,     40 //Reserve
        }
        Method (_DSM, 0x4, Serialized) {
          //Accept only MSF Family type NVDIMM DSM functions
          If(LEqual(Arg0, ToUUID ("1ee68b36-d4bd-4a1a-9a16-4f8e53d46e05"))) {
            //Handle Func 0 query implemented commands
            If(LEqual(Arg2, 0)) {
              //Check revision and returned proper implemented commands
              //Support only health check for now
              Return (Buffer() {0x01, 0x08}) //Byte 0: 0x1
            }
            //Handle MSF DSM Func 11 Get Smart and Health Info
            If(LEqual(Arg2, 11)) {
              Store(\_SB.NVDR.NVD3.STAT, BSTA)
              Store(\_SB.NVDR.NVD3.HLTH, BHTH)
              Store(\_SB.NVDR.NVD3.CTMP, BTMP)
              Store(\_SB.NVDR.NVD3.ETHS, BETH)
              Store(\_SB.NVDR.NVD3.WTHS, BWTH)
              Store(\_SB.NVDR.NVD3.NVLF, BNLF)
              Return (SMRT)
            }
          }
          Return (Buffer() {0})
        }
        Method(_STA, 0, NotSerialized) {
          return (0xf)
        }
      }
      Device (NVD4) {
        Name(_ADR, 0x1770)
        Name(SMRT, Buffer(13) {0})
        CreateDWordField(SMRT, 0, BSTA)
        CreateWordField(SMRT, 4, BHTH)
        CreateWordField(SMRT, 6, BTMP)
        CreateByteField(SMRT, 8, BETH)
        CreateByteField(SMRT, 9, BWTH)
        CreateByteField(SMRT, 10, BNLF)
        OperationRegion(BUF1, SystemMemory, 0xC0088000, 16)
        Field (BUF1, DWordAcc, NoLock, Preserve) {
          STAT, 32, //Status
          HLTH, 16, //Module Health
          CTMP, 16, //Module Current Status
          ETHS, 8, //Error Threshold Status
          WTHS, 8, //Warning Threshold Status
          NVLF, 8, //NVM Lifetime
          ,     40 //Reserve
        }
        Method (_DSM, 0x4, Serialized) {
          //Accept only MSF Family type NVDIMM DSM functions
          If(LEqual(Arg0, ToUUID ("1ee68b36-d4bd-4a1a-9a16-4f8e53d46e05"))) {
            //Handle Func 0 query implemented commands
            If(LEqual(Arg2, 0)) {
              //Check revision and returned proper implemented commands
              //Support only health check for now
              Return (Buffer() {0x01, 0x08}) //Byte 0: 0x1
            }
            //Handle MSF DSM Func 11 Get Smart and Health Info
            If(LEqual(Arg2, 11)) {
              Store(\_SB.NVDR.NVD4.STAT, BSTA)
              Store(\_SB.NVDR.NVD4.HLTH, BHTH)
              Store(\_SB.NVDR.NVD4.CTMP, BTMP)
              Store(\_SB.NVDR.NVD4.ETHS, BETH)
              Store(\_SB.NVDR.NVD4.WTHS, BWTH)
              Store(\_SB.NVDR.NVD4.NVLF, BNLF)
              Return (SMRT)
            }
          }
          Return (Buffer() {0})
        }
        Method(_STA, 0, NotSerialized) {
          return (0xf)
        }
      }
    }

    Device (TPM0) {
      //
      // TPM 2.0
      //

      //
      // TAG for patching TPM2.0 _HID
      //
      Name (_HID, "NNNN0000")
      Name (_CID, "MSFT0101")
      Name (_UID, 0)

      Name (CRBB, 0x10000000)
      Name (CRBL, 0x10000000)

      Name (RBUF, ResourceTemplate () {
        Memory32Fixed (ReadWrite, 0x88500000, 0x1000, PCRE)
      })

      Method (_CRS, 0x0, Serialized) {
        // Declare fields in PCRE
        CreateDWordField(RBUF, ^PCRE._BAS, BASE)
        CreateDWordField(RBUF, ^PCRE._LEN, LENG)

        // Store updatable values into them
        Store(CRBB, BASE)
        Store(CRBL, LENG)

        Return (RBUF)
      }

      Method (_STR,0) {
        Return (Unicode ("TPM 2.0 Device"))
      }

      Method (_STA, 0) {
        if (TPMF) {
          Return (0x0f)  //Enable resources
        }
        Return (0x0)
      }

      //
      // Add opregions for doorbell and PPI CRB
      // The addresses for these operation regions should be patched
      // with information from HOB
      //
      OperationRegion (TPMD, SystemMemory, 0x100000542010, 0x04)
      Field (TPMD, DWordAcc, NoLock, Preserve) {
        DBB0, 32 // Doorbell out register
      }

      // PPI request CRB
      OperationRegion (TPMC, SystemMemory, 0x88542038, 0x0C)
      Field (TPMC, DWordAcc, NoLock, Preserve) {
          PPIO, 32, // current PPI request
          PPIR, 32, // last PPI request
          PPIS, 32, // last PPI request status
      }

      // Create objects to hold return values
      Name (PKG2, Package (2) { Zero, Zero })
      Name (PKG3, Package (3) { Zero, Zero, Zero })

      Method (_DSM, 0x4, Serialized) {
        // Handle Physical Presence Interface(PPI) DSM method
        If (LEqual (Arg0, ToUUID ("3DDDFAA6-361B-4eb4-A424-8D10089D1653"))) {
          Switch (ToInteger (Arg2)) {
            //
            // Standard DSM query
            //
            Case (0) {
              Return (Buffer () { 0xFF, 0x01 })
            }

            //
            // Get Physical Presence Interface Version - support 1.3
            //
            Case (1) {
              Return ("1.3")
            }

            //
            // Submit TPM operation to pre-OS (Deprecated)
            //
            Case (2) {
              Return (One) // Not supported
            }

            //
            // Get pending TPM operation requested by OS
            //
            Case (3) {
              PKG2[Zero] = Zero   // Success
              PKG2[One] = PPIO    // current PPI request
              Return (PKG2)
            }

            //
            // Platform-specific action to transition to Pre-OS env
            //
            Case (4) {
              Return (0x2) // Reboot
            }

            //
            // TPM operation Response to OS
            //
            Case (5) {
              PKG3[Zero] = Zero   // Success
              PKG3[One] = PPIR    // last PPI request
              PKG3[2] = PPIS      // last PPI request status
              Return (PKG3)
            }

            //
            // Preferred language code (Deprecated)
            //
            Case (6) {
              Return (0x3) // Not implemented
            }

            //
            // Submit TPM operation to pre-OS env 2
            //
            Case (7) {
              Local0 = DerefOf (Arg3 [Zero])
              // Write current PPI request and then to the doorbell
              Store (Local0, PPIO)
              Store (0x6a000000, DBB0) // MsgType: 6, Handler: 0xa (TPM-PPI)
              Return (Zero)
            }

            //
            // Get User confirmation status for op
            //
            Case (8) {
              Return (0x4) // Allowed and physically present user not required
            }
          }
        }
        Return (Buffer () {0})
      }
    }

//
//GPIO 0 to 7
//

  Device(GPO0) {
  Name(_HID, "APMC0D07")
  Name(_ADR, 0)
  Name(_UID, 0)

  Name (_CRS, ResourceTemplate ()  {
    //Memory32Fixed (ReadWrite, 0x602E0000, 0x10000)
    QWordMemory (
      ResourceConsumer,
      ,
      MinFixed,
      MaxFixed,
      NonCacheable,
      ReadWrite,
      0x0000000000000000,
      0x00001000026F0000,
      0x00001000026FFFFF,
      0x0000000000000000,
      0x0000000000010000
    )
    Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 476 }
  })

  Device(PRTa) {
    Name(_ADR, 0)
    Name(_UID, 0)
    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"reg", 0},
        Package () {"snps,nr-gpios", 8},
      }
    })
  }

  }

//
//GPIO 16 to 23
//

  Device(GPO1) {
  Name(_HID, "APMC0D07")
  Name(_ADR, 0)
  Name(_UID, 1)

  Name (_CRS, ResourceTemplate ()  {
    //Memory32Fixed (ReadWrite, 0x602E0000, 0x10000)
    QWordMemory (
      ResourceConsumer,
      ,
      MinFixed,
      MaxFixed,
      NonCacheable,
      ReadWrite,
      0x0000000000000000,
      0x00001000027B0000,
      0x00001000027BFFFF,
      0x0000000000000000,
      0x0000000000010000
    )
    Interrupt(ResourceConsumer, Level, ActiveHigh, Exclusive) { 477 }
  })

  Device(PRTa) {
    Name(_ADR, 0)
    Name(_UID, 0)
    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"reg", 0},
        Package () {"snps,nr-gpios", 8},
      }
    })
  }

  }

//
//GPI 0 to 7
//

  Device(GPI0) {
  Name(_HID, "APMC0D07")
  Name(_ADR, 0)
  Name(_UID, 2)

  Name (_CRS, ResourceTemplate ()  {
    //Memory32Fixed (ReadWrite, 0x602E0000, 0x10000)
    QWordMemory (
      ResourceConsumer,
      ,
      MinFixed,
      MaxFixed,
      NonCacheable,
      ReadOnly,
      0x0000000000000000,
      0x00001000026D0000,
      0x00001000026DFFFF,
      0x0000000000000000,
      0x0000000000010000
    )
  })

  Device(PRTa) {
    Name(_ADR, 0)
    Name(_UID, 0)
    Name(_DSD, Package () {
      ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
      Package () {
        Package () {"reg", 0},
        Package () {"snps,nr-gpios", 8},
      }
    })
  }

  }

    Include ("PCI-S0.Rca01.asi")
    Include ("PCI-S0.asi")
    Include ("PCI-S1.asi")
    Include ("PCI-PDRC.asi")
  }
} // DSDT
