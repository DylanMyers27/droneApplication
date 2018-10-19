public class Samples {

    static boolean intakePumpIsOn = true;
    static boolean solutionPumpIsOn = true;


    public static void main(String args[]){
    while(true){
        double[] samples = getSamples(5) ;
        getAverage(samples);

    }


    }


    public static double getAverage(double samples[]){

        double average = 0;

        for(int i = 0; i < samples.length - 1 ; i++){
            average += samples[i];
        }

        average = average/samples.length;
        System.out.println("The average is: " + average);
        return average;
    }

    public static double[] getSamples(int numOfSamples){
        int counter = 0;
        double[] samples = new double[numOfSamples] ;
        flushSystem(5);
        turnOffIntake();
        turnOnPumpA(2);
        turnOffSolution();
        while(counter < numOfSamples){
         //   flushSystem(5);
         //   turnOnPumpA(2);
            System.out.println(counter);
            samples[counter] = getSensorReading() ;
            counter++;
        }
        return samples;
    }
    public static void turnOnPumpA(int time){
        System.out.println("Pump A is turned on for " + time + " seconds.");
    }

    public static void flushSystem(int time){
        System.out.println("Flushing system for " + time + " seconds.");

    }

    public static double getSensorReading(){
       double sample = Math.random() ;
        //System.out.println(sample);
       return sample;
    }
    public static void turnOffIntake(){
        if(intakePumpIsOn){
            // Turn off intake through GPIO
        }
        System.out.println("Intake Pump Off");
    }
    public static void turnOffSolution(){

        if(solutionPumpIsOn){
            // Turn off solution through GPIO
        }
        System.out.println("Solution Pump Off");
    }

}
