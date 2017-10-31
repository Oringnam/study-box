public class TotalAted {
	private static double tCal = 0;
	private static double tCarbo = 0;
	private static double tProt = 0;
	private static double tFat = 0;

	// get
	public static double gettCal()		{ return tCal;	}
	public static double gettCarbo()	{ return tCarbo;}
	public static double gettProt()		{ return tProt; }
	public static double gettFat()		{ return tFat;  }
	// set
	public static void settCal(double tCal)		{ TotalAted.tCal	= tCal;		}
	public static void settCarbo(double tCarbo) { TotalAted.tCarbo 	= tCarbo;	}
	public static void settProt(double tProt)	{ TotalAted.tProt	= tProt;	}
	public static void settFat(double tFat)		{ TotalAted.tFat	= tFat;		}
	
	// adding
	public static void addCal(double tCal)		{ TotalAted.tCal	+= tCal;	}
	public static void addCarbo(double tCarbo)	{ TotalAted.tCarbo	+= tCarbo;	}
	public static void addtFat(double tFat)		{ TotalAted.tFat	+= tFat;	}
	public static void addProt(double tProt)	{ TotalAted.tProt	+= tProt;	}

	// food was added
	public static void add(FoodButton x) {
		tCarbo	+= x.getTansoo();
		tFat 	+= x.getFat();
		tProt 	+= x.getProtein();
		tCal 	+= x.getCalories();
	}//add
	
	// food was deleted
	public static void minus(FoodButtonPanel x) {
		tCarbo	-= x.getTansoo();
		tFat	-= x.getFat();
		tProt	-= x.getProtein();
		tCal 	-= x.getCalories();
	}//minus
}
