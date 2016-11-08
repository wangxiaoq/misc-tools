#define pr_fmt(s) KBUILD_MODNAME ": " s

#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/mce.h>
#include <asm/msr.h>
#include <asm/msr-index.h>

#define CMCI_EN (1UL << 30)

static int dump_mca_init(void)
{
	int i;
	int bank_num;
	u64 mcgcap;
	u64 mci_ctl2;


	rdmsrl(MSR_IA32_MCG_CAP, mcgcap);
	bank_num = (int)(mcgcap & MCG_BANKCNT_MASK);

	pr_info("----------------DUMP MCA START----------------");
	pr_info("Value of IA32_MCG_CAP %llu.", mcgcap);
	pr_info("Number of banks: %d.", bank_num);
	pr_info("IA32_MCG_CTL is %s.", (mcgcap & MCG_CTL_P) ? "present" : "not present");
	pr_info("%s extend machine-check state registers.", (mcgcap & MCG_EXT_P) ? "Support" : "Not support");
	pr_info("%s CMCI.", (mcgcap & MCG_CMCI_P) ? "Support" : "Not support");
//	pr_info("Threshold-based error status reporting %s.", (mcgcap & MCG_TES_P) ? "present" : "not present");
	pr_info("Number of extend machine-check state registers %d.", (int)(mcgcap & MCG_EXT_CNT_MASK));
	pr_info("Software error recoverable %s.", (mcgcap & MCG_SER_P) ? "support" : "not support");
//	pr_info("%s enhanced machine check capability for firmware first signaling.", (mcgcap & MCG_EMC_P) ? "Support" : "Not support");
	pr_info("%s enhanced error logging.", (mcgcap & MCG_ELOG_P) ? "Support" : "Not support");
	pr_info("%s local machine check exception.", (mcgcap & MCG_LMCE_P) ? "Support" : "Not support");

	for (i = 0; i < bank_num; i++) {
		rdmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		mci_ctl2 |= CMCI_EN;
		wrmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		rdmsrl(MSR_IA32_MCx_CTL2(i), mci_ctl2);
		pr_info("Bank %d %s CMCI signal interface.", i, (mci_ctl2 & CMCI_EN) ? "support" : "not support");
	}

	pr_info("----------------DUMP MCA END----------------");

	return 0;
}

static void dump_mca_exit(void)
{
	return ;
}

module_init(dump_mca_init);
module_exit(dump_mca_exit);

MODULE_LICENSE("GPL v2");