/***************************************************************************\
 *
 *   
 *         _/        _/_/_/_/    _/_/    _/      _/   _/_/_/
 *        _/        _/        _/    _/  _/_/    _/         _/
 *       _/        _/_/_/    _/    _/  _/  _/  _/     _/_/
 *      _/        _/        _/    _/  _/    _/_/         _/
 *     _/_/_/_/  _/_/_/_/    _/_/    _/      _/   _/_/_/
 *   
 *
 *
 *   
 *   This file is part of LEON3.
 *   
 *   LEON3 is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *   
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *   or see <http://www.gnu.org/licenses/>.
 *   
 *
 *
 *   (c) Luca Fossati, fossati.l@gmail.com
 *
\***************************************************************************/



#include <leon3.funcat/alias.hpp>
#include <leon3.funcat/registers.hpp>
#include <ostream>
#include <list>

using namespace leon3_funcat_trap;
void leon3_funcat_trap::Alias::immediateWrite( const unsigned int & value ) throw(){
    this->reg->immediateWrite(value);
}

unsigned int leon3_funcat_trap::Alias::readNewValue() throw(){
    return this->reg->readNewValue();
}

unsigned int leon3_funcat_trap::Alias::operator ~() throw(){
    return ~(*this->reg + this->offset);
}

unsigned int leon3_funcat_trap::Alias::operator +( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) + *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator -( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) - *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator *( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) * *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator /( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) / *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator |( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) | *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator &( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) & *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator ^( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) ^ *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator <<( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) << *other.reg);
}

unsigned int leon3_funcat_trap::Alias::operator >>( const Alias & other ) const throw(){
    return ((*this->reg + this->offset) >> *other.reg);
}

Alias & leon3_funcat_trap::Alias::operator =( const Alias & other ) throw(){
    *this->reg = *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator +=( const Alias & other ) throw(){
    *this->reg += *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator -=( const Alias & other ) throw(){
    *this->reg -= *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator *=( const Alias & other ) throw(){
    *this->reg *= *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator /=( const Alias & other ) throw(){
    *this->reg /= *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator |=( const Alias & other ) throw(){
    *this->reg |= *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator &=( const Alias & other ) throw(){
    *this->reg &= *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator ^=( const Alias & other ) throw(){
    *this->reg ^= *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator <<=( const Alias & other ) throw(){
    *this->reg <<= *other.reg;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator >>=( const Alias & other ) throw(){
    *this->reg >>= *other.reg;
    return *this;
}

bool leon3_funcat_trap::Alias::operator <( const Register & other ) const throw(){
    return ((*this->reg + this->offset) < other);
}

bool leon3_funcat_trap::Alias::operator >( const Register & other ) const throw(){
    return ((*this->reg + this->offset) > other);
}

bool leon3_funcat_trap::Alias::operator <=( const Register & other ) const throw(){
    return ((*this->reg + this->offset) <= other);
}

bool leon3_funcat_trap::Alias::operator >=( const Register & other ) const throw(){
    return ((*this->reg + this->offset) >= other);
}

bool leon3_funcat_trap::Alias::operator ==( const Register & other ) const throw(){
    return ((*this->reg + this->offset) == other);
}

bool leon3_funcat_trap::Alias::operator !=( const Register & other ) const throw(){
    return ((*this->reg + this->offset) != other);
}

Alias & leon3_funcat_trap::Alias::operator =( const Register & other ) throw(){
    *this->reg = other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator +=( const Register & other ) throw(){
    *this->reg += other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator -=( const Register & other ) throw(){
    *this->reg -= other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator *=( const Register & other ) throw(){
    *this->reg *= other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator /=( const Register & other ) throw(){
    *this->reg /= other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator |=( const Register & other ) throw(){
    *this->reg |= other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator &=( const Register & other ) throw(){
    *this->reg &= other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator ^=( const Register & other ) throw(){
    *this->reg ^= other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator <<=( const Register & other ) throw(){
    *this->reg <<= other;
    return *this;
}

Alias & leon3_funcat_trap::Alias::operator >>=( const Register & other ) throw(){
    *this->reg >>= other;
    return *this;
}

std::ostream & leon3_funcat_trap::Alias::operator <<( std::ostream & stream ) const \
    throw(){
    stream << *this->reg + this->offset;
    return stream;
}

void leon3_funcat_trap::Alias::directSetAlias( Alias & newAlias ) throw(){
    this->reg = newAlias.reg;
    this->offset = newAlias.offset;
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = &newAlias;
    newAlias.referredAliases.push_back(this);
}

void leon3_funcat_trap::Alias::directSetAlias( Register & newAlias ) throw(){
    this->reg = &newAlias;
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = NULL;
}

leon3_funcat_trap::Alias::Alias( Register * reg, unsigned int offset ) : reg(reg), \
    offset(offset), defaultOffset(0){
    this->referringAliases = NULL;
}

leon3_funcat_trap::Alias::Alias() : offset(0), defaultOffset(0){
    this->referringAliases = NULL;
}

leon3_funcat_trap::Alias::Alias( Alias * initAlias, unsigned int offset ) : reg(initAlias->reg), \
    offset(initAlias->offset + offset), defaultOffset(offset){
    initAlias->referredAliases.push_back(this);
    this->referringAliases = initAlias;
}

leon3_funcat_trap::Alias::~Alias(){
    std::list<Alias *>::iterator referredIter, referredEnd;
    for(referredIter = this->referredAliases.begin(), referredEnd = this->referredAliases.end(); \
        referredIter != referredEnd; referredIter++){
        if((*referredIter)->referringAliases == this){
            (*referredIter)->referringAliases = NULL;
        }
    }
    if(this->referringAliases != NULL){
        this->referringAliases->referredAliases.remove(this);
    }
    this->referringAliases = NULL;
}


