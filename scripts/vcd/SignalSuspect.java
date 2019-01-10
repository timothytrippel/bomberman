package vcd;

public class SignalSuspect extends SignalHistory
{
    private boolean isSuspect = true;
    private long timeLastSuspect = -1;
    
    // Returns true if there are no repeats and the signal is not fully-expressed
    public boolean isSuspect()
    {
	// No longer suspect if repeated values or fully-expressed
	// Use short-circuit eval to avoid expensive ops
	if(isSuspect && (hasRepeatedValues() || isFullyExpressed()))
	{
	    isSuspect = false;
	    timeLastSuspect = getTimeOfLastUpdate();
	}
	
	return isSuspect;
    }

    // Does this signal have repeated values
    public boolean hasRepeatedValues()
    {
	for(int idxA = 0; idxA < getValues().size(); ++idxA)
	{
	    String valA = getValues().get(idxA).getValue();
	    
	    for(int idxB = idxA + 1; idxB < getValues().size(); ++idxB)
	    {
		if(valA.equals(getValues().get(idxB).getValue()))
		    return true;
	    }
	}
	
	return false;
    }

    // Is this signal fully-expressed, i.e., we've seen all possible values
    public boolean isFullyExpressed()
    {
	// Safely ignore signals that are too large
	if(getWidth() > 24)
	    return false;

	return (getValues().size() >= maxValues()) ? true : false;
    }

    // Report when we removed this signal from the pool of suspects
    // Returns -1 if this signal is still a suspect
    public long gitTimeLastSuspect()
    {
	return timeLastSuspect;
    }
    
    /**
     Constructs a signal with the specified properties, when path and name are seperated.
     @author Matthew Hicks
     @param pPath Path in the design hierarchy of the signal. Assumes an ending '/'.
     @param pName Short name of the signal.
     @param pType Signal type; reg or wire.
     @param pWidth Number of bits in the signal.
     @param pSymbol Symbol used in the VCD file to concisely represent this signal.
     @see SignalType
    */
    public SignalSuspect(String pPath, String pName, SignalType pType, int pWidth, String pSymbol)
    {
        super(pPath, pName, pType, pWidth, pSymbol);
    }
    
    /**
     Constructs a signal with the specified properties, when path and name are combined.
     @author Matthew Hicks
     @param pName Full name of the signal.
     @param pType Signal type; reg or wire.
     @param pWidth Number of bits in the signal.
     @param pSymbol Symbol used in the VCD file to concisely represent this signal.
     @see SignalType
    */
    public SignalSuspect(String pName, SignalType pType, int pWidth, String pSymbol)
    {
        super(pName, pType, pWidth, pSymbol);
    }    
  }
