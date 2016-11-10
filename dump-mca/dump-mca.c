#define pr_fmt(s) KBUILD_MODNAME ": " s

#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/mce.h>
#include <asm/msr.h>
#include <asm/msr-index.h>

#define THRESHOLD ((1UL << 15) - 1)

static int dump_mca_init(void)
{
	int i;
	int bank_num;
	u64 mcgcap;
	u64 mci_ctl2;
	u64 mci_ctl2_bak;


	rdmsrl(MSR_IA32_MCG_CAP, mcgcap);
	bank_num = (int)(mcgcap & MCG_BANKCNT_MASK);

	pr_info("----------------DUMP MCA START----------------\n");
	pr_info("Value of IA32_MCG_CAP %llu.\n", mcgcap);
	pr_info("Number of banks: %d.\n", bank_num);
	pr_info("IA32_MCG_CTL is %s.\n", (mcgcap & MCG_CTL_P) ? "present" : "not present");
	pr_info("%s extend machine-check state registers.\n", (mcgcap & MCG_EXT_P) ? "Support" : "Not support");
	pr_info("%s CMCI.\n", (mcgcap & MCG_CMCI_P) ? "Support" : "Not support");
//	pr_info("Threshold-based error status reporting %s.", (mcgcap & MCG_TES_P) ? "present" : "not present");
	pr_info("Number of extend machine-check state registers %d.\n", (int)(mcgcap & MCG_EXT_CNT_MASK));
	pr_info("Software error recoverable %s.\n", (mcgcap & MCG_SER_P) ? "support" : "not support");
//	pr_info("%s enhanced machine check capability for firmware first signaling.", (mcgcap & MCG_EMC_P) ? "Support" : "Not support");
	pr_info("%s enhanced error logging.\n", (mcgcap & MCG_ELOG_P) ? "Support" : "Not support");
	pr_info("%s local machine check exception.\n", (mcgcap & MCG_LMCE_P) ? "Support" : "Not support");

	for (i = 0; i < bank_num; i++) {
		rdmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		mci_ctl2_bak = mci_ctl2;

		mci_ctl2 |= THRESHOLD;
		wrmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		rdmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		pr_info("Bank %d %s CMCI threshold.\n", i, ((mci_ctl2 & MCI_CTL2_CMCI_THRESHOLD_MASK)
					== THRESHOLD) ? "support" : "not support");

		/* restore the MCi_CTL2 to original state */
		wrmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2_bak);

		mci_ctl2 = mci_ctl2_bak;
		mci_ctl2 |= MCI_CTL2_CMCI_EN;
		wrmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		rdmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		pr_info("Bank %d %s CMCI signal interface.\n", i, (mci_ctl2 & MCI_CTL2_CMCI_EN) ? "support" : "not support");

		/* restore */
		wrmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2_bak);
	}

	pr_info("----------------DUMP MCA END----------------\n");

	return 0;
}

static void dump_mca_exit(void)
{
	return ;
}

module_init(dump_mca_init);
module_exit(dump_mca_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Wang Xiaoqiang < wang_xiaoq@126.com >");
