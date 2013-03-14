module Ruloir
  class Template
    DYNAMIC_REGEX=/<%=[ \t\r\n]*([A-Za-z_$0-9@!]+)[ \t\r\n]*(\[[ \t\r\n]*([A-Za-z_$0-9@!]+)[ \t\r\n]*\])?[ \t\r\n]*%>/m
    
    def self.parse_file(file)
      self.parse(File.read(file))
    end
    def self.parse(code)
      static_chunks=code.split(DYNAMIC_REGEX)
      puts code.scan(DYNAMIC_REGEX).inspect
    end
  end
end