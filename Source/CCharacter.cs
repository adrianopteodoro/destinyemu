using System;
using System.Collections.Generic;
using System.Text;

namespace server
{
    public class CCharacter
    {
        public CStats Stats;
        public CPosition Position;

        public CCharacter()
        {
            Stats = new CStats();
            Stats.cHP = 0;
            Stats.cMP = 0;
            Stats.CON = 0;
            Stats.DEX = 0;
            Stats.INT = 0;
            Stats.mHP = 0;
            Stats.mMP = 0;
            Stats.STR = 0;
            Position = new CPosition();
            Position.pCurrent.x = 0;
            Position.pCurrent.y = 0;
            Position.pDestiny.x = 0;
            Position.pDestiny.y = 0;
            Position.pSource.x = 0;
            Position.pSource.y = 0;
        }

        public void ClearItem(CItem item)
        {
            item.ItemID = 0;
            item.EF1 = 0;
            item.EF2 = 0;
            item.EF3 = 0;
            item.EFV1 = 0;
            item.EFV2 = 0;
            item.EFV3 = 0;
        }

        public int GetItemIDwRefine(CItem item)
        {
            byte[] outitemid = new byte[2];
            int resultid = 0;
            int refine = 0;
            Buffer.BlockCopy(BitConverter.GetBytes(item.ItemID), 0, outitemid, 0, 2);
            if (item.EF1 == 43)
            {
                int efvalue = item.EFV1;
                if ((efvalue > 0) && (efvalue < 230))
                {
                    for (int i = 0; i < 100; i++)
                    {
                        if ((efvalue - (i * 10) >= 0) && (efvalue - (i * 10) <= 11))
                        {
                            refine = efvalue - (i * 10);
                            break;
                        }
                    }
                }
                else if (efvalue >= 230)
                {
                    if ((efvalue >= 230) && (efvalue <= 233))
                        refine = 10;
                    if ((efvalue >= 234) && (efvalue <= 237))
                        refine = 11;
                    if ((efvalue >= 238) && (efvalue <= 241))
                        refine = 12;
                    if ((efvalue >= 242) && (efvalue <= 245))
                        refine = 13;
                    if ((efvalue >= 246) && (efvalue <= 249))
                        refine = 14;
                    if ((efvalue >= 250) && (efvalue <= 253))
                        refine = 15;
                    if ((efvalue >= 254) && (efvalue <= 255))
                        refine = 16;
                }
            }
            if (item.EF2 == 43)
            {
                int efvalue = item.EFV2;
                if ((efvalue > 0) && (efvalue < 230))
                {
                    for (int i = 0; i < 100; i++)
                    {
                        if ((efvalue - (i * 10) >= 0) && (efvalue - (i * 10) <= 11))
                        {
                            refine = efvalue - (i * 10);
                            break;
                        }
                    }
                }
                else if (efvalue >= 230)
                {
                    if ((efvalue >= 230) && (efvalue <= 233))
                        refine = 10;
                    if ((efvalue >= 234) && (efvalue <= 237))
                        refine = 11;
                    if ((efvalue >= 238) && (efvalue <= 241))
                        refine = 12;
                    if ((efvalue >= 242) && (efvalue <= 245))
                        refine = 13;
                    if ((efvalue >= 246) && (efvalue <= 249))
                        refine = 14;
                    if ((efvalue >= 250) && (efvalue <= 253))
                        refine = 15;
                    if ((efvalue >= 254) && (efvalue <= 255))
                        refine = 16;
                }
            }
            if (item.EF3 == 43)
            {
                int efvalue = item.EFV3;
                if ((efvalue > 0) && (efvalue < 230))
                {
                    for (int i = 0; i < 100; i++)
                    {
                        if ((efvalue - (i * 10) >= 0) && (efvalue - (i * 10) <= 11))
                        {
                            refine = efvalue - (i * 10);
                            break;
                        }
                    }
                }
                else if (efvalue >= 230)
                {
                    if ((efvalue >= 230) && (efvalue <= 233))
                        refine = 10;
                    if ((efvalue >= 234) && (efvalue <= 237))
                        refine = 11;
                    if ((efvalue >= 238) && (efvalue <= 241))
                        refine = 12;
                    if ((efvalue >= 242) && (efvalue <= 245))
                        refine = 13;
                    if ((efvalue >= 246) && (efvalue <= 249))
                        refine = 14;
                    if ((efvalue >= 250) && (efvalue <= 253))
                        refine = 15;
                    if ((efvalue >= 254) && (efvalue <= 255))
                        refine = 16;
                }
            }
            switch (refine)
            {
                case 1: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0x10); break;
                case 2: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0x20); break;
                case 3: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0x30); break;
                case 4: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0x40); break;
                case 5: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0x60); break;
                case 8: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0x70); break;
                case 9: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0x90); break;
                case 10: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0xA0); break;
                case 11: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0xB0); break;
                case 12: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0xC0); break;
                case 13: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0xD0); break;
                case 14: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0xE0); break;
                case 15: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0xF0); break;
                case 16: outitemid[1] = Convert.ToByte(Convert.ToInt32(outitemid[1]) + 0xF0); break;
            }
            resultid = BitConverter.ToInt16(outitemid, 0);
            return resultid;
        }
    }
}
