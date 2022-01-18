package ControllerConsole;

public class Module
{
	private int modul;
	private String modtx;
	private String categ;
	private String catxt;
	private int mtype;

	public Module()
	{
	}

	public int getModule()
	{
		return modul;
	}

	public void setModule(int module)
	{
		modul = module;
	}

	public String getModuleText()
	{
		return modtx;
	}

	public void setModuleText(String moduleText)
	{
		modtx = moduleText;
	}

	public String getModuleCategory()
	{
		return categ;
	}

	public void setModuleCategory(String moduleCategory)
	{
		categ = moduleCategory;
	}

	public String getModuleCategoryText()
	{
		return catxt;
	}

	public void setModuleCategoryText(String moduleCategoryText)
	{
		catxt = moduleCategoryText;
	}

	public int getModuleType()
	{
		return mtype;
	}

	public void setModuleType(int moduleType)
	{
		mtype = moduleType;
	}
}