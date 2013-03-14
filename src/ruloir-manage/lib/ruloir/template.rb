module Ruloir
  class Template
    DYNAMIC_REGEX_GROUPS=/<%=[ \t\r\n]*([A-Za-z_$0-9@!]+)[ \t\r\n]*(\[[ \t\r\n]*([A-Za-z_$0-9@!]+)[ \t\r\n]*\])?[ \t\r\n]*%>/m
    DYNAMIC_REGEX_NO_GROUPS=/<%=[ \t\r\n]*[A-Za-z_$0-9@!]+[ \t\r\n]*\[[ \t\r\n]*[A-Za-z_$0-9@!]+[ \t\r\n]*\]?[ \t\r\n]*%>/m 
    
    def self.parse_file(file)
      self.parse(File.read(file))
    end
    def self.parse(code)
      self.new(
        code.split(DYNAMIC_REGEX_NO_GROUPS),
        code.scan(DYNAMIC_REGEX_GROUPS).map {|x|
          (x.last==x.first) ? [x.first] : [x.first, x.last]
        }
      )
    end
    
    def initialize(static_chunks, dynamic_chunks)
      @static_chunks=static_chunks
      @dynamic_chunks=dynamic_chunks
    end
    
    def c_code
      code="int *len; const char* data;"
      
      (0..(@static_chunks.length+@dynamic_chunks.length-1)).each do |i|
        (((i&1)==0) ? @static_chunks : @dynamic_chunks)[i/2]
      end
      
      code
    end
  end
end