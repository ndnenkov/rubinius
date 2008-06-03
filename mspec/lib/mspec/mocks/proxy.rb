class MockProxy
  def initialize
    @multiple_returns = nil
    @returning = nil
    @yielding  = []
    @arguments = :any_args
  end

  def count
    @count ||= [:exactly, 0]
  end

  def arguments
    @arguments
  end

  def returning
    if @multiple_returns
      if @returning.size == 1
        @multiple_returns = false
        return @returning = @returning.shift
      end
      return @returning.shift
    end
    @returning
  end

  def times
    self
  end

  def calls
    @calls ||= 0
  end

  def called
    @calls = calls + 1
  end

  def exactly(n)
    @count = [:exactly, n_times(n)]
    self
  end

  def at_least(n)
    @count = [:at_least, n_times(n)]
    self
  end

  def at_most(n)
    @count = [:at_most, n_times(n)]
    self
  end

  def once
    exactly 1
  end

  def twice
    exactly 2
  end

  def any_number_of_times
    @count = [:any_number_of_times, 0]
    self
  end

  def with(*args)
    raise ArgumentError, "you must specify the expected arguments" if args.empty?
    @arguments = *args
    self
  end

  def and_return(*args)
    case args.size
    when 0
      @returning = nil
    when 1
      @returning = args[0]
    else
      @multiple_returns = true
      @returning = args
      count[1] = args.size if count[1] < args.size
    end
    self
  end

  def and_yield(*args)
    @yielding << args
    self
  end
  
  def yielding
    @yielding
  end
  
  def yielding?
    !@yielding.empty?
  end

  private

  def n_times(n)
    case n
    when :once
      1
    when :twice
      2
    else
      Integer n
    end
  end
end
