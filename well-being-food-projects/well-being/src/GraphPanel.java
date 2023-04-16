import java.awt.*;
import javax.swing.*;
import java.util.concurrent.*;
public class GraphPanel extends JPanel {
	private FoodAteBar 	tot,dan,tan,ji;
	private static int totX,danX,tanX,jiX;
	private static int totP,danP,tanP,jiP;
	
	private JLabel	lblT,lblP,lblC,lblF;
	private JLabel	lblTD,lblPD,lblCD,lblFD;
	
	public GraphPanel(){
		setLayout(null);
		setBackground(Constant.backColor);
		setPreferredSize(new Dimension(760,230));
		
		// type
		type();
		
		// graph
		tot = new FoodAteBar(TotalAted.gettCal(),PrivateInfo.getRecomCal());
		tot.setBounds(50,20,630,40);
		add(tot);
		
		dan = new FoodAteBar(TotalAted.gettCarbo(),PrivateInfo.getRecomProt());
		dan.setBounds(50,70,630,40);
		add(dan);		
		
		tan = new FoodAteBar(TotalAted.gettProt(),PrivateInfo.getRecomCarbo());
		tan.setBounds(50,120,630,40);
		add(tan);
		
		ji = new FoodAteBar(TotalAted.gettFat(),PrivateInfo.getRecomFat());
		ji.setBounds(50,170,630,40);
		add(ji);
		
		// counting		
		lblTD = new JLabel();
		lblPD = new JLabel();
		lblCD = new JLabel();
		lblFD = new JLabel();		
		
		lblTD.setText(totP+"%");
		lblPD.setText(danP+"%");
		lblCD.setText(tanP+"%");
		lblFD.setText(jiP+"%");
		
		lblTD.setBounds(690,10,35,70);
		lblPD.setBounds(690,60,35,70);
		lblCD.setBounds(690,110,35,70);
		lblFD.setBounds(690,160,35,70);
		
		lblTD.setHorizontalAlignment(SwingConstants.CENTER);
		lblTD.setVerticalAlignment(SwingConstants.CENTER);
		
		lblPD.setHorizontalAlignment(SwingConstants.CENTER);
		lblPD.setVerticalAlignment(SwingConstants.CENTER);
		
		lblCD.setHorizontalAlignment(SwingConstants.CENTER);
		lblCD.setVerticalAlignment(SwingConstants.CENTER);
		
		lblFD.setHorizontalAlignment(SwingConstants.CENTER);
		lblFD.setVerticalAlignment(SwingConstants.CENTER);
		
		Font fnt = new Font("Segoe Print",Font.BOLD,11);
		
		lblTD.setFont(fnt);
		lblPD.setFont(fnt);
		lblCD.setFont(fnt);
		lblFD.setFont(fnt);
		
		add(lblTD);
		add(lblPD);
		add(lblCD);
		add(lblFD);	
	}	// GraphPanel()
	
	// Graph's type  // Total, Protein, Carbohydrate, Fat
	public void type() {
		lblT = new JLabel("Cal");
		lblP = new JLabel("Pro");
		lblC = new JLabel("Car");
		lblF = new JLabel("Fat");
		
		lblT.setBounds(5,20,35,40);
		lblP.setBounds(5,70,35,40);
		lblC.setBounds(5,120,35,40);
		lblF.setBounds(5,170,35,40);
		
		lblT.setHorizontalAlignment(SwingConstants.CENTER);
		lblT.setVerticalAlignment(SwingConstants.CENTER);
		
		lblP.setHorizontalAlignment(SwingConstants.CENTER);
		lblP.setVerticalAlignment(SwingConstants.CENTER);
		
		lblC.setHorizontalAlignment(SwingConstants.CENTER);
		lblC.setVerticalAlignment(SwingConstants.CENTER);
		
		lblF.setHorizontalAlignment(SwingConstants.CENTER);
		lblF.setVerticalAlignment(SwingConstants.CENTER);
		
		Font fnt = new Font("Segoe Print",Font.BOLD,17);
		
		lblT.setFont(fnt);
		lblP.setFont(fnt);
		lblC.setFont(fnt);
		lblF.setFont(fnt);
		
		add(lblT);
		add(lblP);
		add(lblC);
		add(lblF);		
	}	// type()
	
	// Counting the ated data
	public void countD() {	
		// percentage return
		totP	= tot.getPercent();
		danP	= dan.getPercent();
		tanP	= tan.getPercent();
		jiP		= ji.getPercent();
		
		// write
		lblTD.setText(totP+"%");
		lblPD.setText(danP+"%");
		lblCD.setText(tanP+"%");
		lblFD.setText(jiP+"%");
		
		// color
		if(totP>100)	lblTD.setForeground(Color.red);
		else 			lblTD.setForeground(Color.black);
		if(danP>100)	lblPD.setForeground(Color.red);
		else 			lblPD.setForeground(Color.black);
		if(tanP>100)	lblCD.setForeground(Color.red);
		else 			lblCD.setForeground(Color.black);
		if(jiP>100)		lblFD.setForeground(Color.red);
		else 			lblFD.setForeground(Color.black);
	}	// countD
	
	// Make a thread. 	// Repainting graph
	public void startThread() {			
		tot.setThis(this);
		dan.setThis(this);
		tan.setThis(this);
		ji.setThis(this);
		
		// restart thread
		tot.renew(totX, totP, TotalAted.gettCal(),PrivateInfo.getRecomCal());
		dan.renew(danX, danP, TotalAted.gettProt(),PrivateInfo.getRecomProt());
		tan.renew(tanX, tanP, TotalAted.gettCarbo(),PrivateInfo.getRecomCarbo());
		ji.renew(jiX, jiP, TotalAted.gettFat(),PrivateInfo.getRecomFat());	
		
		// graph bar size return
		totX	= tot.getDestiny(TotalAted.gettCal(),PrivateInfo.getRecomCal());
		danX	= dan.getDestiny(TotalAted.gettProt(),PrivateInfo.getRecomProt());
		tanX	= tan.getDestiny(TotalAted.gettCarbo(),PrivateInfo.getRecomCarbo());
		jiX		= ji.getDestiny(TotalAted.gettFat(),PrivateInfo.getRecomFat());
	}	// startThread()
}	// GraphPanel class