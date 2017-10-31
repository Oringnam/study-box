
public class PrivateInfo {
	private static int age;
	private static int sex;
	private static int weight;
	private static int height;
	private static double recomCal;
	private static double recomCarbo;
	private static double recomProt;
	private static double recomFat;
		
	// private information
	public PrivateInfo(){};
	public PrivateInfo(int age, int sex, int weight, int height) {
		PrivateInfo.age = age;
		PrivateInfo.sex = sex;
		PrivateInfo.weight = weight;
		PrivateInfo.height = height;
	}	// PrivateInfo()
	
	// privateinfo get/set
	public static int getAge()			{ return age; }	
	public static int getSex()			{ return sex; }
	public static int getWeight()		{ return weight;}	
	public static int getHeight() 		{ return height; }		

	public static void setAge(int age)				{ PrivateInfo.age = age;	}
	public static void setSex(int sex)				{ PrivateInfo.sex = sex;	}
	public static void setWeight(int weight)		{ PrivateInfo.weight = weight; }
	public static void setHeight(int height)		{ PrivateInfo.height = height; }
		
	// recom
	public static void recommend() {
		//탄수화물
		recomCarbo = 4 * PrivateInfo.weight;
		
		//지방
		if(PrivateInfo.sex == 1)	//	man
			recomFat = 58.9;
		else			// woman
			recomFat = 43.0;
		
		//칼로리, 단백질
		if (PrivateInfo.sex == 1) {	// man
			if (PrivateInfo.age>=1 && PrivateInfo.age<=3) {
				recomCal = 1200;
				recomProt = 25;
			}
			else if (PrivateInfo.age>=4 && PrivateInfo.age<=6){
				recomCal = 1600;
				recomProt = 30;
			}
			else if (PrivateInfo.age>=7 && PrivateInfo.age<=9){
				recomCal = 1800;
				recomProt = 40;
			}
			else if (PrivateInfo.age>=10 && PrivateInfo.age<=12){
				recomCal = 2200;
				recomProt = 55;
			}
			else if (PrivateInfo.age>=13 && PrivateInfo.age<=19){
				recomCal = 2700;
				recomProt = 75;
			}
			else if (PrivateInfo.age>=20 && PrivateInfo.age<=49){
				recomCal = 2500;
				recomProt = 70;
			}
			else if (PrivateInfo.age>=50 && PrivateInfo.age<=64){
				recomCal = 2300;
				recomProt = 70;
			}
			else {
				recomCal = 1900;
				recomProt = 60;
			}			
		}	// man
		else {	// woman
			if (PrivateInfo.age>=1 && PrivateInfo.age<=3) {
				recomCal = 1200;
				recomProt = 25;
			}
			else if (PrivateInfo.age>=4 && PrivateInfo.age<=6) {
				recomCal = 1600;
				recomProt = 30;
			}
			else if (PrivateInfo.age>=7 && PrivateInfo.age<=9){
				recomCal = 1800;
				recomProt = 40;
			}
			else if ((PrivateInfo.age>=10 && PrivateInfo.age<=12)||(PrivateInfo.age>=20 && PrivateInfo.age<=49)){
				recomCal = 2000;
				recomProt = 55;
			}
			else if (PrivateInfo.age>=13 && PrivateInfo.age<=19){
				recomCal = 2100;
				recomProt = 65;
			}
			else if (PrivateInfo.age>=50 && PrivateInfo.age<=64){
				recomCal = 1900;
				recomProt = 55;
			}
			else {
				recomCal = 1800;
				recomProt = 55;
			}
		}	// woman
	}	// recommend()
	
	// Recom get/set
	public static double getRecomCal()		{ return recomCal;}	
	public static double getRecomCarbo()	{ return recomCarbo; }	
	public static double getRecomProt()		{ return recomProt; }	
	public static double getRecomFat()		{ return recomFat; }
	
	public static void setRecomCal(double recomCal)			{ PrivateInfo.recomCal = recomCal; }
	public static void setRecomCarbo(double recomCarbo) 	{ PrivateInfo.recomCarbo = recomCarbo; }
	public static void setRecomProt(double recomProt)		{ PrivateInfo.recomProt = recomProt; }
	public static void setRecomFat(double recomFat)			{ PrivateInfo.recomFat = recomFat; }	
}
